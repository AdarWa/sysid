"""
Modified from PyKit
"""

from typing import Any, Optional
import sys
import threading
import time

from pykit.logdatareciever import LogDataReciever
from pykit.logreplaysource import LogReplaySource
from pykit.logtable import LogTable


class _ConsoleRecorder:
    def __init__(self, orig):
        self.orig = orig
        self.lock = threading.Lock()
        self.buffer = ""

    def write(self, s):
        try:
            with self.lock:
                self.orig.write(s)
                try:
                    self.orig.flush()
                except (OSError, ValueError):
                    pass

                self.buffer += s
                while "\n" in self.buffer:
                    line, self.buffer = self.buffer.split("\n", 1)
                    try:
                        Logger.recordOutput("Console", line)
                    except (AttributeError, RuntimeError, ValueError):
                        pass
        except (OSError, ValueError, RuntimeError):
            pass

    def flush(self):
        if self.buffer:
            Logger.recordOutput("Console", self.buffer)
            self.buffer = ""
        try:
            self.orig.flush()
        except (OSError, ValueError):
            pass


class Logger:
    replaySource: Optional[LogReplaySource] = None
    running: bool = False
    cycleCount: int = 0
    entry: LogTable = LogTable(0)
    outputTable: LogTable = LogTable(0)
    metadata: dict[str, str] = {}
    checkConsole: bool = True

    _orig_stdout: Optional[Any] = None
    _orig_stderr: Optional[Any] = None
    _console_wrapped: bool = False
    _console_recorder_stdout: Optional[Any] = None
    _console_recorder_stderr: Optional[Any] = None

    dataRecievers: list[LogDataReciever] = []

    @classmethod
    def setReplaySource(cls, replaySource: LogReplaySource):
        cls.replaySource = replaySource

    @classmethod
    def isReplay(cls) -> bool:
        return cls.replaySource is not None

    @classmethod
    def recordOutput(cls, key: str, value: Any, unit: Optional[str] = None):
        if cls.running:
            cls.entry.put(key, value, unit=unit)

    @classmethod
    def recordMetadata(cls, key: str, value: str):
        if not cls.isReplay():
            cls.metadata[key] = value

    @classmethod
    def addDataReciever(cls, reciever: LogDataReciever):
        cls.dataRecievers.append(reciever)

    @classmethod
    def start(cls):
        if not cls.running:
            cls.running = True
            cls.cycleCount = 0
            print("Logger started")

            if cls.isReplay():
                if cls.replaySource is not None:
                    cls.replaySource.start()
                print("Logger in replay mode")
                cls.outputTable = cls.entry.getSubTable("ReplayOutputs")
            else:
                print("Logger in normal logging mode")
                cls.outputTable = cls.entry.getSubTable("RealOutputs")

            metadataTable = cls.entry.getSubTable(
                "ReplayMetadata" if cls.isReplay() else "RealMetadata"
            )

            for key, value in cls.metadata.items():
                metadataTable.put(key, value)

            if cls.checkConsole and not cls._console_wrapped:
                try:
                    cls._orig_stdout = sys.stdout
                    cls._orig_stderr = sys.stderr
                    cls._console_recorder_stdout = _ConsoleRecorder(cls._orig_stdout)
                    cls._console_recorder_stderr = _ConsoleRecorder(cls._orig_stderr)
                    sys.stdout = cls._console_recorder_stdout
                    sys.stderr = cls._console_recorder_stderr
                    cls._console_wrapped = True
                except (AttributeError, RuntimeError, TypeError):
                    pass

            for reciever in cls.dataRecievers:
                reciever.start()

    @classmethod
    def end(cls):
        if cls.running:
            cls.running = False
            print("Logger ended")

            if cls._console_wrapped:
                try:
                    if cls._orig_stdout is not None:
                        sys.stdout = cls._orig_stdout
                    if cls._orig_stderr is not None:
                        sys.stderr = cls._orig_stderr
                except (AttributeError, RuntimeError):
                    pass
                cls._console_wrapped = False
                cls._console_recorder_stdout = None
                cls._console_recorder_stderr = None
                cls._orig_stdout = None
                cls._orig_stderr = None

            if cls.isReplay() and cls.replaySource is not None:
                cls.replaySource.end()

            for reciever in cls.dataRecievers:
                reciever.end()

    @classmethod
    def getTimestamp(cls) -> int:
        if cls.isReplay():
            return cls.entry.getTimestamp()
        # Returns current time in microseconds, mimicking FPGA time resolution
        return cls.cycleCount * 20000

    @classmethod
    def step(cls):
        if not cls.running:
            return

        cls.cycleCount += 1

        if not cls.isReplay():
            cls.entry.setTimestamp(cls.getTimestamp())
        else:
            rs = cls.replaySource
            if rs is None or not rs.updateTable(cls.entry):
                print("End of replay reached")
                cls.end()
                raise SystemExit(0)

        for reciever in cls.dataRecievers:
            reciever.putTable(LogTable.clone(cls.entry))
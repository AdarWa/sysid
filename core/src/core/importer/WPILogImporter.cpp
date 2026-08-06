//
// Created by adarw on 8/3/26.
//

#include "WPILogImporter.hpp"

#include <iostream>

#ifdef WITH_WPIUTIL
#include <wpi/datalog/DataLogReader.hpp>
#include "wpi/util/DenseMap.hpp"
#endif

namespace sysid {

#ifdef WITH_WPIUTIL

    using EntryMap = wpi::util::DenseMap<int, wpi::log::StartRecordData>;

    static constexpr std::string POSITION_TOPIC = ""; // TODO
    static constexpr std::string VELOCITY_TOPIC = ""; // TODO
    static constexpr std::string ACCEL_TOPIC = ""; // TODO
    static constexpr std::string VOLTAGE_TOPIC = ""; // TODO
    static constexpr std::string STATE_TOPIC = ""; // TODO

    static constexpr std::string DOUBLE_TYPE = "double";
    static constexpr std::string STRING_TYPE = "string";

    static constexpr long WPILIB_DT = 20; // ms

    static bool isVerbose = false;

    static constexpr double toSeconds(const uint64_t timestamp) {
        return static_cast<double>(timestamp) / 1000000.0;
    }

    static constexpr int64_t toMs(const int64_t us) {
        return us / 1000;
    }

    static SystemState stringToState(const std::string_view& state) {
        static const std::unordered_map<std::string_view, SystemState> stateMap = {
            {"DYNAMIC_FORWARD", SystemState::DYNAMIC_FORWARD},
            {"DYNAMIC_BACKWARD", SystemState::DYNAMIC_BACKWARD},
            {"QUASISTATIC_FORWARD", SystemState::QUASISTATIC_FORWARD},
            {"QUASISTATIC_BACKWARD", SystemState::QUASISTATIC_BACKWARD}
        };

        if (const auto it = stateMap.find(state); it != stateMap.end()) {
            return it->second;
        }

        throw std::invalid_argument(std::format("Unknown system state: ", state));
    }

    static void handleStartRecord(const wpi::log::DataLogRecord& record, EntryMap& entries) {
        wpi::log::StartRecordData data;
        if (record.GetStartData(&data)) {
            if (isVerbose)
            std::cout << std::format("Start({}, name='{}', type='{}', metadata='{}') [{}]\n",
                             data.entry, data.name, data.type, data.metadata,
                             toSeconds(record.GetTimestamp())) << std::endl;

            if (entries.find(data.entry) != entries.end()) {
                std::cout << "...DUPLICATE entry ID, overriding" << std::endl;
            }
            entries[data.entry] = data;
        } else {
            std::cerr << "Start(INVALID)" << std::endl;
        }
    }

    static void handleFinishRecord(const wpi::log::DataLogRecord& record, EntryMap& entries) {
        int entry;
        if (record.GetFinishEntry(&entry)) {
            if (isVerbose) {
                std::cout << std::format("Finish({}) [{}]", entry, toSeconds(record.GetTimestamp())) << std::endl;
            }

            auto it = entries.find(entry);
            if (it == entries.end()) {
                std::cerr << std::format("End ID {} not found", entry) << std::endl;
            } else {
                entries.erase(it);
            }
        } else {
            std::cerr << "Finish(INVALID)" << std::endl;
        }
    }

    static void handleMetadataRecord(const wpi::log::DataLogRecord& record, EntryMap& entries) {
        wpi::log::MetadataRecordData data;
        if (record.GetSetMetadataData(&data)) {
            if (isVerbose) {
                std::cout << std::format("SetMetadata({}, '{}') [{}]",
                                 data.entry, data.metadata, toSeconds(record.GetTimestamp())) << std::endl;
            }

            auto it = entries.find(data.entry);
            if (it == entries.end()) {
                std::cerr << std::format("Metadata ID {} not found", data.entry) << std::endl;
            } else {
                it->second.metadata = data.metadata;
            }
        } else {
            std::cerr << "Metadata(INVALID)" << std::endl;
        }
    }

    static bool isPositionRecord(const std::string& name, const std::string& type) {
        return name == POSITION_TOPIC && type == DOUBLE_TYPE;
    }

    static bool isVelocityRecord(const std::string& name, const std::string& type) {
        return name == VELOCITY_TOPIC && type == DOUBLE_TYPE;
    }

    static bool isAccelRecord(const std::string& name, const std::string& type) {
        return name == ACCEL_TOPIC && type == DOUBLE_TYPE;
    }

    static bool isVoltageRecord(const std::string& name, const std::string& type) {
        return name == VOLTAGE_TOPIC && type == DOUBLE_TYPE;
    }

    static bool isStateRecord(const std::string& name, const std::string& type) {
        return name == STATE_TOPIC && type == STRING_TYPE;
    }

    static bool isSysIdRecord(const std::string& name, const std::string& type) {
        return isPositionRecord(name, type) ||
            isVelocityRecord(name, type) ||
                isAccelRecord(name, type) ||
                    isVoltageRecord(name, type) ||
                        isStateRecord(name, type);
    }

    template <typename T>
    static void insertPairSorted(std::vector<std::pair<int64_t, T>>& log, const std::pair<int64_t, T>& newSample) {
        const auto it = std::ranges::lower_bound(log, newSample,
           [](const std::pair<int64_t, T>& a, const std::pair<int64_t, T>& b) {
               return a.first < b.first;
           });

        log.insert(it, newSample);
    }

    template <typename T>
    static void insertLogSorted(const SampleLog& log, const std::vector<std::pair<int64_t, T>>& sampleVec, const std::function<void(size_t, T)>& insertCallback) {
        auto currentIt = log.samples.begin();

        for (const auto& [timestamp, value] : sampleVec) {
            // Start the binary search from the last place. Cool optimization!
            currentIt = std::ranges::lower_bound(
                currentIt,
                log.samples.end(),
                timestamp,
                std::less<>{},
                &LoggedSample::timestamp
            );

            if (currentIt != log.samples.end() && currentIt->timestamp == timestamp) {
                const size_t index = std::distance(log.samples.begin(), currentIt);
                insertCallback(index, value);
            }
        }
    }

    namespace {
        struct VectorSampleLog {
            // <timestamp, state>
            std::vector<std::pair<int64_t, double>> position;
            std::vector<std::pair<int64_t, double>> velocity;
            std::vector<std::pair<int64_t, double>> accel;
            std::vector<std::pair<int64_t, double>> voltage;
            std::vector<std::pair<int64_t, SystemState>> state;

            void sortAll() {
                using DoublePair = std::pair<int64_t, double>;
                using StatePair = std::pair<int64_t, SystemState>;

                std::ranges::sort(position, {}, &DoublePair::first);
                std::ranges::sort(velocity, {}, &DoublePair::first);
                std::ranges::sort(accel, {}, &DoublePair::first);
                std::ranges::sort(voltage, {}, &DoublePair::first);
                std::ranges::sort(state, {}, &StatePair::first);
            }
        };
    }

    template <typename  T>
    static std::pair<long, T> makeNewSample(const wpi::log::DataLogRecord& record, T value) {
        return std::make_pair(toMs(record.GetTimestamp()), value);
    }

    static void handleDataRecord(const wpi::log::DataLogRecord& record, const EntryMap& entries, VectorSampleLog& log) {
        if (isVerbose) {
            std::cout << std::format("Data({}, size={}) ", record.GetEntry(), record.GetSize());
        }

        const auto entry = entries.find(record.GetEntry());
        if (entry == entries.end()) {
            std::cerr << std::format("<ID {} not found>",record.GetEntry()) << std::endl;
            return;
        }

        if (isVerbose) {
            std::cout << std::format("<name='{}', type='{}'> [{}]", entry->second.name, entry->second.type, toSeconds(record.GetTimestamp())) << std::endl;
        }

        const auto name = entry->second.name.data();
        const auto dataType = entry->second.type.data();

        const auto parseFailWarning = [&name]() {
            std::cerr << std::format("Couldn't parse double for {}", name) << std::endl;
        };

        if (isPositionRecord(name, dataType)) {
            double position;
            if (!record.GetDouble(&position)) {
                parseFailWarning();
                return;
            }
            insertPairSorted(log.position, makeNewSample(record, position));
        }else if (isVelocityRecord(name, dataType)) {
            double velocity;
            if (!record.GetDouble(&velocity)) {
                parseFailWarning();
                return;
            }
            insertPairSorted(log.velocity, makeNewSample(record, velocity));
        }else if (isAccelRecord(name, dataType)) {
            double accel;
            if (!record.GetDouble(&accel)) {
                parseFailWarning();
                return;
            }
            insertPairSorted(log.accel, makeNewSample(record, accel));
        }else if (isVoltageRecord(name, dataType)) {
            double voltage;
            if (!record.GetDouble(&voltage)) {
                parseFailWarning();
                return;
            }
            insertPairSorted(log.voltage, makeNewSample(record, voltage));
        }else if (isStateRecord(name, dataType)) {
            std::string_view stateStr;
            if (!record.GetString(&stateStr)) {
                parseFailWarning();
                return;
            }
            const SystemState state = stringToState(stateStr);
            insertPairSorted(log.state, makeNewSample(record, state));
        }
    }

    static void fillVoltageSampleLog(SampleLog& log, const std::vector<std::pair<int64_t, double>>& voltage) {
        for (const auto& [timestamp, v] : voltage) {
            log.samples.push_back(LoggedSample(timestamp, SystemState::DYNAMIC_FORWARD, v, 0, 0, 0));
        }
    }

    static std::vector<std::pair<SystemState, SampleLog>> groupByStatePeriods(const SampleLog& log)
    {
        std::vector<std::pair<SystemState, SampleLog>> result;
        if (log.samples.empty()) {
            return result;
        }

        for (const auto& sample : log.samples) {
            if (result.empty() || result.back().first != sample.systemState) {
                result.push_back({sample.systemState, {{sample}}});
            } else {
                result.back().second.samples.push_back(sample);
            }
        }

        return result;
    }

    static void fillSorted(SampleLog& log, const SystemType& system, const VectorSampleLog& vectorSampleLog) {
        insertLogSorted<double>(log, vectorSampleLog.position, [&log, &system](const size_t it, const double record) {
            if (system == SystemType::POSITIONAL)
                log.samples[it].y_meas = record;
        });

        insertLogSorted<double>(log, vectorSampleLog.velocity, [&log, &system](const size_t it, const double record) {
            if (system == SystemType::POSITIONAL)
                log.samples[it].dydt_meas = record;
            else
                log.samples[it].y_meas = record;
        });

        insertLogSorted<double>(log, vectorSampleLog.accel, [&log, &system](const size_t it, const double record) {
            if (system == SystemType::POSITIONAL)
                log.samples[it].dydt2_meas = record;
            else
                log.samples[it].dydt_meas = record;
        });

        insertLogSorted<SystemState>(log, vectorSampleLog.state, [&log](const size_t it, const SystemState record) {
            log.samples[it].systemState = record;
        });
    }

    static LogFile handleImport(const std::string& filePath) {
        std::expected<std::unique_ptr<wpi::util::MemoryBuffer>, std::error_code> fileBuffer = wpi::util::MemoryBuffer::GetFile(filePath);
        if (!fileBuffer) {
            throw std::runtime_error(std::format("Couldn't open file {}", filePath));
        }
        const wpi::log::DataLogReader reader{std::move(*fileBuffer)};
        if (!reader) {
            throw std::runtime_error(std::format("{} is not a wpilog file!", filePath));
        }

        VectorSampleLog vectorSampleLog;
        EntryMap entries;

        for (auto&& record : reader) {
            if (record.IsStart()) {
                handleStartRecord(record, entries);
            } else if (record.IsFinish()) {
                handleFinishRecord(record, entries);
            } else if (record.IsSetMetadata()) {
                handleMetadataRecord(record, entries);
            } else if (record.IsControl()) {
                std::cerr << "Unrecognized control record" << std::endl;
            } else {
                handleDataRecord(record, entries, vectorSampleLog);
            }
        }

        vectorSampleLog.sortAll();

        constexpr SystemType system = {}; // TODO

        SampleLog log;
        fillVoltageSampleLog(log, vectorSampleLog.voltage);

        fillSorted(log, system, vectorSampleLog);

        log = log.alignTimestamps(WPILIB_DT);

        LogFile logFile;
        logFile.steps = groupByStatePeriods(log);

        return logFile;
    }

#endif

    LogFile WPILogImporter::importFile(const std::string& filePath, const bool verbose) {
#ifndef WITH_WPIUTIL
        throw std::runtime_error("SysId must be built with WPI support to parse WPILogs!");
#else
        isVerbose = verbose;
        return handleImport(filePath);
#endif
    }
} // sysid
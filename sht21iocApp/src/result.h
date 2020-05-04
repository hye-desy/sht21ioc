#pragma once

#include <map>
#include <mutex>

/** Measurements available for one channel. */
struct result {
    float dew_point;
    float humidity;
    float temperature;
    bool ok = false;
};


class result_list {
    public:
		static result_list* instance();
		result get_result(uint8_t _device);
		void update_result(uint8_t _device, result&& _result);

    protected:
		std::map<uint8_t, result> m_results;
		std::mutex m_result_mutex;
};

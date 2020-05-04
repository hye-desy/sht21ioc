#include "result.h"
#include "gpios.h"

#include <iostream>

result_list* result_list::instance()
{
    static result_list self;
    return &self;
}

result result_list::get_result(uint8_t _device)
{
    std::lock_guard<std::mutex> lock(m_result_mutex);
    return m_results[_device];
}

void result_list::update_result(uint8_t _device, result&& _result)
{
    {
		std::lock_guard<std::mutex> lock(m_result_mutex);
		m_results.erase(_device);
		m_results.emplace(_device, _result);
    }
    ::scanIoRequest(pvts[_device]);
}

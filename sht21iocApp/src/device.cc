#include <aiRecord.h>
#include <dbScan.h>
#include <devSup.h>
#include <epicsExport.h>
#include <map>

#include "gpios.h"
#include "result.h"

std::map<std::string, float result::*> parameters = {
    { "dew", &result::dew_point },
    { "rh", &result::humidity },
    { "temp", &result::temperature },
};

class SHT21Device {
    public:
	SHT21Device(aiRecord* _record)
	    : m_record(_record)
	{
	    m_valid = true;
	    if (m_record->inp.value.vmeio.card != 1) {
		m_valid = false;
	    }

	    const auto& it = parameters.find(m_record->inp.value.vmeio.parm);
	    if (it == parameters.end()) {
		m_valid = false;
	    } else {
		m_value = it->second;
	    }

	}

	::IOSCANPVT getIoIntInfo() {
	    return pvts[m_record->inp.value.vmeio.signal];
	}

	long read()
	{
	    if (!m_valid) {
		m_record->udf = 1;
		return 2;
	    }

	    result r = result_list::instance()->get_result(m_record->inp.value.vmeio.signal);
	    if (r.ok) {
		m_record->udf = 0;
		m_record->val = r.*m_value;
	    } else {
		m_record->udf = 1;
	    }
	    return 2;
	}

    protected:
	::aiRecord* const m_record;
	bool m_valid;
	float result::* m_value;
};


static long init_record_ai(void *_record)
{
    ::aiRecord* ai = static_cast<::aiRecord*>(_record);
    ai->dpvt = new SHT21Device(ai);
    return 0;
}

static long read_record_ai(void *_record)
{
    ::aiRecord* ai = static_cast<::aiRecord*>(_record);
    SHT21Device* dev = static_cast<SHT21Device*>(ai->dpvt);
    return dev->read();
}

static long get_ioint_info_ai(int _cmd, void* _record, IOSCANPVT* _pvt)
{
    ::aiRecord* ai = static_cast<::aiRecord*>(_record);
    SHT21Device* dev = static_cast<SHT21Device*>(ai->dpvt);
    *_pvt = dev->getIoIntInfo();
    return 0;
}

struct
{
    long num;
    DEVSUPFUN report;
    DEVSUPFUN init;
    DEVSUPFUN init_record;
    DEVSUPFUN get_ioint_info;
    DEVSUPFUN read_ai;
    DEVSUPFUN special_linconv;
} DevAiSHT21 = {
    6, /* 6 functions */
    nullptr, //report
    nullptr, // init
    init_record_ai, // init_record
    reinterpret_cast<DEVSUPFUN>(get_ioint_info_ai), // get_ioint_info
    read_record_ai, // read_ai
    nullptr // special_linconv
};

epicsExportAddress(dset, DevAiSHT21);


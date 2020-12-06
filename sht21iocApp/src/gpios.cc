 #include "gpios.h"

std::map<uint32_t, ::IOSCANPVT> pvts;

//const std::vector<uint32_t> gpio{17, 27};
const std::vector<uint32_t> gpio{17, 27, 22};

namespace {

		class initializer
		{
			public:
				initializer()
				{
					for(const auto g : gpio) {
						::IOSCANPVT pvt;
						::scanIoInit(&pvt);
						pvts.emplace(g, pvt);
			}
		}
} instance;

}


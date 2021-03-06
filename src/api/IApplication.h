#ifndef _iapplication_h_
#define _iapplication_h_
namespace egal
{
	class IApplication
	{
	public:
		virtual int init() = 0;
		virtual int uninit() = 0;

		virtual int run() = 0;

		virtual int exit() = 0;
	};
}
#endif

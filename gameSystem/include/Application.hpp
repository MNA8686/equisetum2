#if !defined(_EQAPPLICATION_H_)
#define _EQAPPLICATION_H_

#include "Equisetum2.h"
using namespace Equisetum2; 

#include "SystemView.hpp"
#include "Dashboard.hpp"

class Application
{
public:
	Application() = default;
	~Application() = default;

	int Main();

	void SetTargetFps(int32_t fps);
	int32_t NowFps(void) const;

	std::shared_ptr<Renderer>& GetRenderer(void);
	std::shared_ptr<FontManager>& GetSystemFont(void);

protected:

	virtual bool OnCreate(void) = 0;
	virtual bool OnInit(void) = 0;
	virtual void OnQuit(void) = 0;
	virtual bool OnUpdate(void) = 0;
	virtual bool OnDraw(void) = 0;

	virtual bool OnActive(bool active);
	
	virtual String GetOrganizationName(void) = 0;
	virtual String GetApplicationName(void) = 0;

	void Quit();

private:
	std::shared_ptr<Dashboard> m_dashboard;

	std::shared_ptr<Renderer> m_renderer;
	std::shared_ptr<FontManager> m_fontManager;

	std::shared_ptr<SystemTimer> m_sysTimer;
	std::shared_ptr<TickCounter> m_fpsMakerTick;
	std::shared_ptr<FpsPaceMaker> m_fpsMaker;
	std::shared_ptr<TickCounter> m_fpsCounterTick;
	std::shared_ptr<FpsCounter> m_fpsCounter;

	int32_t m_fps = 60;
	int32_t m_systemFontSize = 8;
	bool m_isQuit = false;
};

extern Application* GetApplication(void);

#endif

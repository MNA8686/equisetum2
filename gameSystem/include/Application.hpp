#if !defined(_EQAPPLICATION_H_)
#define _EQAPPLICATION_H_

#include "Equisetum2.h"
using namespace Equisetum2; 

class Application
{
public:
	Application() = default;
	~Application() = default;

	int Main();

protected:

	virtual bool OnInit(void) = 0;
	virtual void OnQuit(void) = 0;
	virtual bool OnUpdate(void) = 0;
	virtual bool OnDraw(std::shared_ptr<Renderer>& renderer) = 0;

	virtual bool OnActive(bool active);

	void Quit();

	void SetTargetFps(int32_t fps);
	int32_t NowFps(void) const;

	std::shared_ptr<Renderer>& GetRenderer(void);

private:

	std::shared_ptr<Renderer> m_renderer;

	std::shared_ptr<SystemTimer> m_sysTimer;
	std::shared_ptr<TickCounter> m_fpsMakerTick;
	std::shared_ptr<FpsPaceMaker> m_fpsMaker;
	std::shared_ptr<TickCounter> m_fpsCounterTick;
	std::shared_ptr<FpsCounter> m_fpsCounter;

	int32_t m_fps = 60;
	bool m_isQuit = false;
};

extern std::shared_ptr<Application> GetApplication(void);

#endif

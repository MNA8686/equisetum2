#include "system/Logger.h"
#include "system/Exception.hpp"
#include "SystemView.hpp"

#if 0
std::shared_ptr<SystemView> SystemView::CreateSystemView()
{
	EQ_DURING
	{
		auto inst = std::shared_ptr<SystemView>(new SystemView);
		if (!inst)
		{
			EQ_THROW(u8"インスタンスの作成に失敗しました。");
		}


		return inst;
	}
	EQ_HANDLER
	{
		Logger::OutputError(EQ_GET_HANDLER().what());
	}
	EQ_END_HANDLER

	return nullptr;
}
#endif

SystemView::SystemView()
{

}

SystemView::~SystemView()
{

}

void SystemView::SetPos(const Point& pos)
{

}

const String& SystemView::GetName() const
{
	return{};
}

int SystemView::Enter()
{
	return 0;
}

int SystemView::Leave()
{
	return 0;
}

void Do()
{
	//	ウィジェットの操作
}

class RootMenu : public SystemView
{
public:
	RootMenu() = default;
	virtual ~RootMenu() = default;

	int Enter() override
	{
		m_returnToGame = SystemWidgetNext::Create();
		m_vWidget.push_back(m_returnToGame);

		m_inputTest = SystemWidgetNext::Create([]() {
			return nullptr;
		});
		m_vWidget.push_back(m_inputTest);

		m_assetTest = SystemWidgetNext::Create();
		m_vWidget.push_back(m_assetTest);

		return 0;
	}

	void Do()
	{
	//	ウィジェットの操作
	}

	std::shared_ptr<RootMenu> Create();

protected:
	std::shared_ptr<SystemWidget> m_returnToGame;
	std::shared_ptr<SystemWidget> m_inputTest;
	std::shared_ptr<SystemWidget> m_assetTest;
};

class AssetMenu : public SystemView
{
public:
	AssetMenu() = default;
	virtual ~AssetMenu() = default;

	int Enter() override
	{
		auto m_returnToGame = SystemWidgetSpin::Create([](int32_t val) {
			
		});
		m_vWidget.push_back(m_returnToGame);

		auto m_prev = SystemWidgetPrev::Create();
		m_vWidget.push_back(m_prev);

		return 0;
	}

	void Do()
	{
	//	ウィジェットの操作
	}

	std::shared_ptr<AssetMenu> Create();

protected:
	//std::shared_ptr<SystemWidget> m_returnToGame;
	//std::shared_ptr<SystemWidget> m_inputTest;
	//std::shared_ptr<SystemWidget> m_assetTest;
};

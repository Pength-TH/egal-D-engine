
#include "runtime/app.h"

#include "common/egal-d.h"
#include "common/filesystem/binary.h"

#include "runtime/EngineFramework/scene_manager.h"

#include <bgfx/platform.h>

namespace egal  /** 初始化引擎 */
{
	IEngine* IEngine::create(const egal_params& param)
	{
		IEngine* app = new App(param);
		return app;
	}

	/** 卸载引擎,关闭所有系统 */
	void IEngine::destroy(IEngine* engine)
	{
		SAFE_DELETE(engine);
	}
}


namespace egal
{
	App::App(const egal_params& param)
		: m_allocator(m_main_allocator)
		, m_width(1024)
		, m_height(768)
		, m_async_op(FALSE)
		, m_frame_scale_time(1.0f)
		, m_p_engine_root(nullptr)
	{
		g_allocator = &m_main_allocator;

		/** 先初始化文件系统 */
		init_file_system(m_allocator);
		
		/** logger */
		init_log();
		log_info("egal engine init start...");

		e_char current_dir[MAX_PATH_LENGTH];
#ifdef PLATFORM_WINDOWS
		GetCurrentDirectory(sizeof(current_dir), current_dir);
#else
		current_dir[0] = '\0';
#endif

		m_p_frame_timer = Timer::create(m_allocator);
		log_info("current dir: %s.\n if the dir error, may be too long.", current_dir);

		/** 初始化引擎 相关系统 */
		m_p_engine_root = EngineRoot::create(current_dir, "", g_file_system, m_allocator);
		if (!m_p_engine_root)
		{
			log_error("engine init failed.");
			return;
		}

		/**设置窗口*/
		EngineRoot::PlatformData platform_data = {};
		platform_data.window_handle = param.hWnd;
		m_p_engine_root->setPlatformData(platform_data);

		/**初始化组件管理器*/
		m_p_component_manager = &m_p_engine_root->createComponentManager();
		
		for (int i = 0; i < KC_BOARD_END; i++)
			g_key_board[i] = false;

		for (int i = 0; i < MB_END; i++)
			g_mouse_board[i] = false;
	}

	App::~App()
	{
		for (int i = 0; i < KC_BOARD_END; i++)
			g_key_board[i] = false;

		for (int i = 0; i < MB_END; i++)
			g_mouse_board[i] = false;

		m_p_engine_root->destroyComponentManager(*m_p_component_manager);

		EngineRoot::destroy(m_p_engine_root, m_allocator);

		Timer::destroy(m_p_frame_timer);
		
		close_log();

		destory_file_system(m_allocator);
	}

	void App::init()
	{

	}

	e_bool App::resize(uint32_t width, uint32_t height)
	{
		m_p_engine_root->getRender().resize(width, height);
		return TRUE;
	}

	e_bool App::run()
	{
		float frame_time = m_p_frame_timer->tick();
		m_p_engine_root->frame();

		if (frame_time < 1 / 100.0f)
		{
			//PROFILE_BLOCK("sleep");
			MT::sleep(e_uint32(1000 / 100.0f - frame_time * 1000));
		}


		return TRUE;
	}

	e_bool App::pause()
	{

		return TRUE;
	}

	e_bool App::resume()
	{

		return TRUE;
	}

	bool App::on_mouse_moved(double x, double y, double distance)
	{
		m_p_engine_root->on_mouse_moved(x, y);

		return TRUE;
	}

	bool App::on_mouse_pressed(const MouseButtonID &mouseid, double distance)
	{
		return TRUE;
	}

	bool App::on_mouse_released(const MouseButtonID &mouseid, double distance)
	{

		return TRUE;
	}

	bool App::on_key_pressed(unsigned int textid, const KeyCode &keyCode)
	{
		egal::SceneManager* m_p_scene_manager = m_p_engine_root->getSceneManager();
		ComponentHandle cmp_hand = m_p_scene_manager->getCameraInSlot("main");
		GameObject m_camera = m_p_scene_manager->getCameraGameObject(cmp_hand);

		return TRUE;
	}

	bool App::on_key_released(unsigned int textid, const KeyCode &keyCode)
	{

		return TRUE;
	}

	void App::update(double timeSinceLastFrame)
	{

	}
}
#pragma once

#include "cmmn.h"

namespace qeg
{
#ifdef WIN32
	//timer
	// Helper class for basic timing.
	class timer
	{
	public:
		// Initializes internal timer values.
		timer()
		{
			if (!QueryPerformanceFrequency(&m_frequency))
			{
				throw exception("QueryPerfFreq failed");
			}
			reset();
		}

		// Reset the timer to initial values.
		void reset()
		{
			update();
			m_startTime = m_currentTime;
			m_total = 0.0f;
			m_delta = 1.0f / 60.0f;
		}

		// Update the timer's internal values.
		void update()
		{
			if (!QueryPerformanceCounter(&m_currentTime))
			{
				throw exception("QueryPerfCoun failed");
			}

			m_total = static_cast<float>(
				static_cast<double>(m_currentTime.QuadPart - m_startTime.QuadPart) /
				static_cast<double>(m_frequency.QuadPart)
				);

			if (m_lastTime.QuadPart == m_startTime.QuadPart)
			{
				// If the timer was just reset, report a time delta equivalent to 60Hz frame time.
				m_delta = 1.0f / 60.0f;
			}
			else
			{
				m_delta = static_cast<float>(
					static_cast<double>(m_currentTime.QuadPart - m_lastTime.QuadPart) /
					static_cast<double>(m_frequency.QuadPart)
					);
			}

			m_lastTime = m_currentTime;
		}

		// Duration in seconds between the last call to Reset() and the last call to Update().
		inline float total_time()
		{
			return m_total;
		}

		// Duration in seconds between the previous two calls to Update().
		inline float delta_time()
		{
			return m_delta;
		}

	private:
		//these are really bad names!
		LARGE_INTEGER m_frequency;
		LARGE_INTEGER m_currentTime;
		LARGE_INTEGER m_startTime;
		LARGE_INTEGER m_lastTime;
		float m_total;
		float m_delta;
	};
#endif

	//trigger_timer
	//triggers a function after some amount of time
	class trigger_timer
	{
		float _t;
		float _max_t;
		function<void()> _action;
		bool _reo;
		bool _done;
	public:
		trigger_timer(float max, const function<void()>& act, bool rec = false)
			: _t(0.f), _max_t(max), _action(act), _reo(rec), _done(false)
		{
		}

		void update(float dt)
		{
			if (_done) return;
			_t += dt;
			if(_t >= _max_t)
			{
				_action();
				_t = 0.f;
				if(!_reo)
				{
					_done = true;
				}
			}
		}

		void reset()
		{
			_t = 0.f;
			_done = false;
		}

		proprw(float, max_time, { return _max_t; });
		propr(float, time, { return _t; });
		proprw(function<void()>, action, { return _action; });
		proprw(bool, reoccuring, { return _reo; });
		propr(bool, done, { return _done; });
	};
}
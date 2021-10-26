#include "common.hpp"
#include "invoker.hpp"
#include "crossmap.hpp"
#include "pointers.hpp"
#include "script.hpp"

namespace big
{
	native_call_context::native_call_context()
	{
		m_return_value = &m_return_stack[0];
		m_args = &m_arg_stack[0];
	}

	void native_invoker::cache_handlers()
	{
		for (const rage::scrNativeMapping& mapping : g_crossmap)
		{
			rage::scrNativeHandler handler = g_pointers->m_get_native_handler(
				g_pointers->m_native_registration_table, mapping.second);

			m_handler_cache.emplace(mapping.first, handler);
		}
	}

	void native_invoker::begin_call() { m_call_context.reset(); }

	void native_invoker::end_call(rage::scrNativeHash hash)
	{
		TRY_CLAUSE
		{
			if (auto it = m_handler_cache.find(hash); it != m_handler_cache.end())
			{
				rage::scrNativeHandler handler = it->second;

				handler(&m_call_context);

				//m_fix_vectors is what is causing the crashes (mostly involving vehicles)

				if (g_pointers->m_fix_vectors != nullptr)
				{
					g_pointers->m_fix_vectors(&m_call_context);
					/*
						this is what the fix vector function is 
				
					
						__int64 __fastcall sub_7FF6B2639A50(__int64 a1)
						{
							__int64 v2; // rax
							__int64 v3; // rcx
							__int64 result; // rax

							for (; *(DWORD*)(a1 + 24); *(DWORD*)(v3 + 16) = result)
							{
								--* (DWORD*)(a1 + 24);
								**(DWORD**)(a1 + 8i64 * *(int*)(a1 + 24) + 32) = *(DWORD*)(a1 + 16 * (*(int*)(a1 + 24) + 4i64));
								*(DWORD*)(*(uint64_t*)(a1 + 8i64 * *(int*)(a1 + 24) + 32) + 8i64) = *(DWORD*)(a1 + 16i64 * *(int*)(a1 + 24) + 68);
								v2 = *(int*)(a1 + 24);
								v3 = *(uint64_t*)(a1 + 8 * v2 + 32);
								result = *(unsigned int*)(a1 + 16 * v2 + 72);
							}
							--*(DWORD*)(a1 + 24);
							return result;
						}					
					*/
				}
				else
				{
					g_pointers->m_fix_vectors = 0x0;
					g_running = false;
				}
			}
			else
				[hash]() { LOG(WARNING) << "Failed to find " << (hash) << "'s native handler."; }();
		}
		EXCEPT_CLAUSE
	}
}
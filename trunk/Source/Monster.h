#pragma once

#include "Common.h"

namespace TLMP {

  static PVOID spider_mgr_this = 0;

  void _spider_some_create_pre STDARG;
  void _spider_some_create_post STDARG;

  void _spider_process_ai_pre STDARG;

  void _spider_process_ai2_pre STDARG;

  void _spider_idle_pre STDARG;

  void _spider_on_hit_pre STDARG;

};

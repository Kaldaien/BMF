/**
* This file is part of Batman "Fix".
*
* Batman "Fix" is free software : you can redistribute it and / or modify
* it under the terms of the GNU General Public License as published by
* The Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* Batman "Fix" is distributed in the hope that it will be useful,
* But WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with Batman "Fix". If not, see <http://www.gnu.org/licenses/>.
**/

#ifndef __BMF__IO_MONITOR_H__
#define __BMF__IO_MONITOR_H__

#include <Windows.h>

struct io_perf_t {
  bool           init          = false;

  ULARGE_INTEGER last_update;
  IO_COUNTERS    accum;
  ULONGLONG      dt;
  IO_COUNTERS    last_counter;

  double         read_mb_sec   = 0.0;
  double         write_mb_sec  = 0.0;
  double         other_mb_sec  = 0.0;

  double         read_iop_sec  = 0.0;
  double         write_iop_sec = 0.0;
  double         other_iop_sec = 0.0;
};

void BMF_CountIO (io_perf_t& ioc, const double update = 0.25 / 1.0e-7);

#define _WIN32_DCOM
#include <Wbemidl.h>

struct WMI_refresh_thread_t
{
  HANDLE                   hThread                      = 0;

  IWbemRefresher          *pRefresher                   = nullptr;
  IWbemConfigureRefresher *pConfig                      = nullptr;
  IWbemHiPerfEnum         *pEnum                        = nullptr;
  IWbemObjectAccess      **apEnumAccess                 = nullptr;
  long                     lID                          = 0;

  DWORD                    dwNumObjects                 = 0;
  DWORD                    dwNumReturned                = 0;
};

#include <stdint.h>

struct cpu_perf_t : WMI_refresh_thread_t
{
  long                     lPercentInterruptTimeHandle  = 0;
  long                     lPercentPrivilegedTimeHandle = 0;
  long                     lPercentUserTimeHandle       = 0;
  long                     lPercentProcessorTimeHandle  = 0;
  long                     lPercentIdleTimeHandle       = 0;

  // Why 64-bit integers are needed for percents is beyond me,
  //   but this is WMI's doing not mine.
  struct {
    uint64_t               percent_load                 = 0;
    uint64_t               percent_idle                 = 0;
    uint64_t               percent_kernel               = 0;
    uint64_t               percent_user                 = 0;
    uint64_t               percent_interrupt            = 0;

    uint64_t               temp_c                       = 0;
  } cpus [64];

  DWORD                    num_cpus                     = 0;
};


struct disk_perf_t : WMI_refresh_thread_t
{
  long                     lNameHandle                  = 0;

  long                     lDiskBytesPerSecHandle       = 0;
  long                     lDiskReadBytesPerSecHandle   = 0;
  long                     lDiskWriteBytesPerSecHandle  = 0;

  long                     lPercentDiskReadTimeHandle   = 0;
  long                     lPercentDiskWriteTimeHandle  = 0;
  long                     lPercentDiskTimeHandle       = 0;
  long                     lPercentIdleTimeHandle       = 0;

  struct {
    char                   name [32];

    uint64_t               percent_load                 = 0;
    uint64_t               percent_write                = 0;
    uint64_t               percent_read                 = 0;
    uint64_t               percent_idle                 = 0;

    uint64_t               read_bytes_sec               = 0;
    uint64_t               write_bytes_sec              = 0;
    uint64_t               bytes_sec                    = 0;
  } disks [16];

  DWORD                    num_disks                    = 0;
};


struct pagefile_perf_t : WMI_refresh_thread_t
{
  long                     lNameHandle                  = 0;

  long                     lPercentUsageHandle          = 0;
  long                     lPercentUsagePeakHandle      = 0;
  long                     lPercentUsage_BaseHandle     = 0;

  struct {
    char                   name [256];

    DWORD                  size                         = 0;
    DWORD                  usage                        = 0;
    DWORD                  usage_peak                   = 0;
  } pagefiles [16];

  DWORD                    num_pagefiles                = 0;
};


DWORD WINAPI BMF_MonitorCPU      (LPVOID user);
DWORD WINAPI BMF_MonitorDisk     (LPVOID user);
DWORD WINAPI BMF_MonitorPagefile (LPVOID user);

extern cpu_perf_t      cpu_stats;
extern disk_perf_t     disk_stats;
extern pagefile_perf_t pagefile_stats;

#endif /* __ BMF__IO_MONITOR_H__ */
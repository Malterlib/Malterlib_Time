# CLAUDE.md - Time Module

This file provides guidance to Claude Code (claude.ai/code) when working with the Time module in Malterlib.

## Module Overview

The Time module provides comprehensive time measurement, manipulation, and timing facilities for the Malterlib framework. It offers high-precision time representation, calendar operations, performance measurement tools, and cross-platform timing utilities.

### Key Features
- High-precision time representation with fractional seconds (using atomic clock precision)
- Calendar operations with Proleptic Gregorian and ISO Week calendar systems
- Time zone support with UTC/Local time conversions
- Performance measurement tools (timers, clocks, cycle counters)
- Cross-platform implementation supporting Windows, macOS, Linux, and Emscripten
- Time formatting and parsing utilities
- Time span calculations and manipulations

## Architecture

### Core Components

#### Time Representation (CTime)
- **Location**: `Source/Malterlib_Time_Time.h`
- **Purpose**: Represents absolute time points with high precision
- **Key Features**:
  - 64-bit seconds since epoch (-7,514,938,706-11-23 Gregorian Proleptic)
  - 64-bit fraction representing sub-second precision (9,223,372,031,757,829,470 units per second)
  - Support for dates from year -7,514,938,705 to 577,039,110,548
  - Invalid time state handling

#### Time Span (CTimeSpan)
- **Location**: `Source/Malterlib_Time_Time.h`
- **Purpose**: Represents duration between two time points
- **Key Features**:
  - Signed 64-bit seconds with fractional component
  - Arithmetic operations (addition, subtraction, multiplication, division)
  - Conversion utilities for various time units

#### Calendar Systems
- **CTimeConvert_ProlepticGreogrian**: Default calendar system with leap year calculations
- **CTimeConvert_ISOWeek**: ISO 8601 week date system
- **CTimeConvert_BabylonianCommon**: Base calendar conversion utilities

#### Timer Classes
- **Location**: `Source/Malterlib_Time_Timer.h`
- **Components**:
  - `CTimer`: Standard timer for elapsed time measurement
  - `CClock`: Wall clock timer with start/stop functionality
  - `CCycles`: CPU cycle-based timer for high-precision measurements
  - `CTimerMin`: Minimum time tracker for performance optimization
  - `CStopWatch`: Simple stopwatch implementation
  - `CTimeout`: Timeout detection utility

### Platform Layer
- **Location**: `Source/Platform/`
- **Files**:
  - `Malterlib_Time_Platform_Windows.cpp`: Windows implementation using QueryPerformanceCounter
  - `Malterlib_Time_Platform_MacOS.cpp`: macOS implementation using mach_absolute_time
  - `Malterlib_Time_Platform_Linux.cpp`: Linux implementation using clock_gettime
  - `Malterlib_Time_Platform_Emscripten.cpp`: WebAssembly implementation
  - `Malterlib_Time_Platform_clang.hpp`: Clang-specific optimizations
  - `Malterlib_Time_Platform_MSVC.hpp`: MSVC-specific optimizations

## Usage Patterns

### Creating and Manipulating Time

```cpp
// Create specific time
CTime Time = CTimeConvert::fs_CreateTime(2024, 12, 25, 14, 30, 0, 0.5);

// Get current time
CTime Now = CTime::fs_NowUTC();
CTime LocalNow = CTime::fs_NowLocal();

// Time arithmetic
CTimeSpan OneDay = CTimeSpanConvert::fs_CreateDaySpan(1);
CTime Tomorrow = Now + OneDay;

// Extract date/time components
CTimeConvert::CDateTime DateTime;
CTimeConvert(Time).f_ExtractDateTime(DateTime);
```

### Performance Measurement

```cpp
// Basic timing
CTimer Timer;
Timer.f_Start();
// ... code to measure ...
Timer.f_Stop();
fp64 Elapsed = Timer.f_GetTime();

// Scope-based timing
{
	DMibScopeTimer(Timer);
	// ... code to measure ...
} // Timer stops automatically

// Minimum time tracking
CTimerMin MinTimer;
for (mint i = 0; i < 100; ++i)
{
	MinTimer.f_Start();
	// ... code to measure ...
	MinTimer.f_Stop();
}
fp64 MinTime = MinTimer.f_GetTime();
```

### Time Formatting

```cpp
CTime Time = CTime::fs_NowLocal();
CStr TimeStr = fg_GetFullTimeStr(Time);      // Full precision string
CStr IsoStr = fg_GetISO8601TimeStr(Time);     // ISO 8601 format
CStr AscStr = fg_GetAscTimeStr(Time);         // C library asctime format

// Custom formatting with options
CTime::CFormatOptions Options;
Options.m_bFullPrecision = true;
Options.m_DateSeparator = '/';
```

### Time Literals

```cpp
// Using time literals for readable code
fp64 Delay = 100_ms;        // 100 milliseconds
fp64 Timeout = 5_seconds;   // 5 seconds
fp64 Duration = 2.5_hours;  // 2.5 hours
fp64 Period = 7_days;       // 7 days
```

## Important Constants

### Time Limits
- **Maximum Year**: 577,039,110,548
- **Minimum Year**: -7,514,938,705
- **Invalid Time Seconds**: 0xffffffffffffffff
- **Invalid Span Seconds**: 0x7fffffffffffffff
- **Fraction Dividend**: 9,223,372,031,757,829,470 (atomic clock precision)

### Time Units
- **Seconds in Day**: 86,400
- **Seconds in Hour**: 3,600
- **Seconds in Minute**: 60
- **Days in Median Year**: 365
- **Days in Leap Year**: 366

## Dependencies

- **Malterlib_Numeric**: For numeric operations (indirect dependency)
- **Core**: Core Malterlib functionality
- **Contract**: Contract programming support
- **Atomic**: Atomic operations for thread-safe timing
- **Memory**: Memory management utilities
- **String**: String formatting and manipulation

## Testing

Test files are located in `Test/`:
- `Test_Malterlib_Time_Time.cpp`: Core time functionality tests
- `Test_Malterlib_Time_SafeTimer.cpp`: Safe timer tests

### Running Tests
```bash
./mib generate Tests
./mib build Tests macOS x86_64 Debug
/opt/Deploy/Tests/RunAllTests --paths '["Time/Test_Malterlib_Time_Time"]'
```

## Platform-Specific Considerations

### Windows
- Uses QueryPerformanceCounter for high-resolution timing
- Handles time zone conversions via Windows API

### macOS
- Uses mach_absolute_time for cycle counting
- Leverages system clock_gettime when available

### Linux
- Uses clock_gettime with CLOCK_MONOTONIC for timing
- CLOCK_REALTIME for wall clock time

### Emscripten (WebAssembly)
- Limited precision due to browser security restrictions
- Uses performance.now() for timing

## Thread Safety

- CTime and CTimeSpan are value types and thread-safe for reading
- Timer classes should be used per-thread or protected with synchronization
- Platform timing functions are thread-safe

## Performance Considerations

- Use CCycles for ultra-low overhead timing measurements
- CTimerMin automatically tracks minimum times to filter out system noise
- Cycle timers include correction factors to account for measurement overhead
- For production timing, prefer CTimer over CCycles (more portable)

## Common Pitfalls

1. **Time Zone Confusion**: Always be explicit about UTC vs Local time
2. **Leap Years**: The module correctly handles leap years including century rules
3. **Precision Loss**: When converting to floating-point seconds, some precision may be lost
4. **Invalid Times**: Always check f_IsValid() when receiving times from external sources
5. **Platform Differences**: Timer resolution varies by platform (check fs_GetResolution())

## Debug Features

When DMibDebuggerHelpers is defined:
- Time and TimeSpan objects have debug string representations
- Additional validation checks are enabled

## Integration with String Formatting

The Time module integrates with Malterlib's string formatting system:
- CTime supports custom format options via f_Format()
- Formatting options include precision control, component selection, and separator customization
- See Documentation/Malterlib_Time.dox for formatting option details

## Module Configuration

Build configuration in `Malterlib_Time.MHeader`:
- Module is part of the Malterlib library collection
- Can be built separately with MalterlibLibrary_Time property
- Includes automated test discovery when MalterlibEnableInternalTests is true

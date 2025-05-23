### Key features
- auto generate header files
- use separate makefile for testing under (project)/Host_Tests


### Directions (See Getting Started Below for Dependencies)
1) Write tests in Host_Tests/test_util.c/main
2) In the `Host_Tests` directory simply run `make tests`, and check the terminal output for results


### File Structure
nucleo-F411RE-base/              # Project root (also STM32CubeIDE workspace)
├── Core/
│   ├── Inc/
│   │   └── util.h               # Auto-generated header (from util.c)
│   │   └── (other files)
│   └── Src/
│       └── app.c               # User application code (not used for host testing)
│       └── main.c               # CubeIDE generated main.c (not used for host testing)
│       └── util.c               # Shared logic for embedded + host **Key Point**: We want to test this on the host machine, not the embedded target
│
├── Host_Tests/                  # Host-only test folder
│   ├── test_util.c             # Main test runner (with assertions)
│   ├── Makefile.test           # Builds + runs host tests
│   ├── test_runner             # (output binary created by make)
│   └── HelperScripts/
│       └── gen_util_h.py       # Python script to auto-generate util.h
│
├── Makefile                    # Root Makefile (optional entry point)
└── .vscode/                    # (optional) for build tasks or launch configs



### Dependencies

VSCode installed
VSCode plugins:
- none required
Git installed (for version control)



### Notes

Need to manually refresh STM32CubeIDE to see the newly created `Inc/util.h` file (`Fn5` should refresh)
# Build

#### 1. How To use build.py script

##### Arguments

| Flags                 | Description                                                                           |
|--------------------   |-------------------------------------------------------------------------------------- |
|  --fbx-location   	  | directory where FBX SDK is located.                                             	    |
| workspace_location    | directory where the project use as a workspace to build and install examples  		    |

##### Stages

| Flag                 | Description                                                                            |
|--------------------  |--------------------------------------------------------------------------------------- |
|  --stages            | comma-separated list of stages: "clean, configure, build, install"                     |

| Options       | Description                                                                                   |
|-----------    |---------------------------------------------------                                            |
| clean         | clean build                                                                                   |
| configure     | call this stage every time a cmake file is modified                                           |
| build         | builds the project                                                                            |
| install       | installs the project                                                                          |

example
```
--stages=configure,build,install
```
***NOTE:*** All the flags can be followed by either ```space``` or ```=```

##### Variant

supported build variants: debug, release, relWithDebInfo can be passed via `--variant` flag.

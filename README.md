# min.jit
A place to stash experiments with using min-devkit to create Jitter objects.

Currently the only object (min.jit.testgen) demonstrates how to create a generator type Jitter object with min. It loads a text file of vertex data and outputs the data as a matrix.


## Prerequisites

You can use the objects provided in this package as-is.

To code your own objects, or to re-compile existing objects, you will need a compiler:

* On the Mac this means **Xcode 9 or later** (you can get from the App Store for free). 
* On Windows this means **Visual Studio 2017** (you can download a free version from Microsoft). The installer for Visual Studio 2017 offers an option to install Git, which you should choose to do.

You will also need the Min-DevKit, available from the Package Manager inside of Max or [directly from Github](https://github.com/Cycling74/min-devkit).

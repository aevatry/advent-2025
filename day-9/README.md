This is the ninth day of the advent of code calendar.   
To build and run, navigate to the correct directory in a terminal and run ```make run```. This has only been tested on MacOS with clang version 17.0.0.

For the part 1 of this problem, we just need to not check for boundary crossing in our validation function (scrap the validation function all together

Still 1 major problem with the current algorithm: if a rectangle makes a line, that is not handled well.   
Also the sorting is slow because there is a bunch of 0's in the array. Could be fixed with keeping a variable with the current maximum area.

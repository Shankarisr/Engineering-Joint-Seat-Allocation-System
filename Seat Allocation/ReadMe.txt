The source code for the project "JOINT SEAT ALLOCATION SYSTEM" is in the file SeatAllocationProject.c.

NOTE:
administrative/ admin no is always 12345
password for admin login is 345.

"student_rank.csv" contains the details of the registered students along with their mains and advanced rank.

"student_matrix (2).csv" contains the application no, mains rank, advanced rank and the choices of the students which was filled by executing the above source code.

For review purpose, to see how the Login and Choice filling takes place, we have "Login-Choice_sample.c", which verifies the login for 2 students and takes their choices and stores it in "sample.csv".

NOTE:
The password for any registered student is the last 3 digits of their application number.

In the "SeatAllocationProject.c", during allocation, "student+matrix (2).csv" is fed so that correct output can be seen.

Once the allocation process is done, the allocation details are stored in "student_allocation.csv".

This file is read and the detials of students alloted in college is displayed along with the no.of seats left(which will be 0).
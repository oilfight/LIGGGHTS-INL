cl__1 = 1;
Point(1) = {-0.5, -0.5, 0, 1};
Point(2) = {-0.5, 0.5, 0, 1};
Point(3) = {0.5, 0.5, 0, 1};
Point(4) = {0.5, -0.5, 0, 1};
Line(1) = {1, 4};
Line(2) = {4, 3};
Line(3) = {3, 2};
Line(4) = {2, 1};
Line Loop(6) = {4, 1, 2, 3};
Plane Surface(6) = {6};

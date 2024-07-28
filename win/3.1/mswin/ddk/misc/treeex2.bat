expand -r %1\*._
if exist %1\*._ del %1\*._
expand -r %1\*.?_
if exist %1\*.?_ del %1\*.?_
expand -r %1\*.??_
if exist %1\*.??_ del %1\*.??_

A tool for quickly importing every modules installed in the current environment.  
Usage:  
Before executing the `Import()` function, you can change some specific attributes.  
To enable/disable debug/error report, you can change `qie._debug` and `qie.suppress_errors`.  
To skip some specific modules, please make changes to the list `qie._skip_modules`.  
After everything is ready, you can run the following:  
```
from qie import *  
Import()  
```  
And it will work!  
For default, the module will print some logging lines to display errors.  
You can change this action by changing `qie._debug` and `qie.suppress_errors`. 
  
Update log:  
  
1.0.1 2024/10/6 The first version

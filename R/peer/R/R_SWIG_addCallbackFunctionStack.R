R_SWIG_addCallbackFunctionStack <-
function(fun, userData = NULL)
{
    # No PACKAGE argument as we don't know what the DLL is.
  .Call("R_SWIG_R_pushCallbackFunctionData", fun, userData)
}


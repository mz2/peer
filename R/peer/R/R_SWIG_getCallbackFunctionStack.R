R_SWIG_getCallbackFunctionStack <-
function()
{
    # No PACKAGE argument as we don't know what the DLL is.
  .Call("R_SWIG_debug_getCallbackFunctionData")
}


VBFA <-
structure(function()
{
  ;ans = .Call('R_swig_new_VBFA', PACKAGE='peer');
  class(ans) <- "_p_PEER__cVBFA";
  
  reg.finalizer(ans, delete_VBFA)
  ans
  
}
, returnType = "_p_PEER__cVBFA", class = c("SWIGFunction", "character"
))

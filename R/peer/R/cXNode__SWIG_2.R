cXNode__SWIG_2 <-
structure(function(E1, prior_offset, prior_prec)
{
  ;ans = .Call('R_swig_new_cXNode__SWIG_2', E1, prior_offset, prior_prec, PACKAGE='peer');
  class(ans) <- "_p_PEER__cXNode";
  
  reg.finalizer(ans, delete_cXNode)
  ans
  
}
, returnType = "_p_PEER__cXNode", inputTypes = c("_p_PMatrix", 
"_p_PMatrix", "_p_PMatrix"), class = c("SWIGFunction", "character"
))

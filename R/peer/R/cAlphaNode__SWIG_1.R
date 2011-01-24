cAlphaNode__SWIG_1 <-
structure(function(dim, pa, pb, E1)
{
  dim = as.integer(dim); 
  
  if(length(dim) > 1) {
    warning("using only the first element of dim");
  };
  
  
  
  
  ;ans = .Call('R_swig_new_cAlphaNode__SWIG_1', dim, pa, pb, E1, PACKAGE='peer');
  class(ans) <- "_p_PEER__cAlphaNode";
  
  reg.finalizer(ans, delete_cAlphaNode)
  ans
  
}
, returnType = "_p_PEER__cAlphaNode", inputTypes = c("integer", 
"numeric", "numeric", "_p_PMatrix"), class = c("SWIGFunction", 
"character"))

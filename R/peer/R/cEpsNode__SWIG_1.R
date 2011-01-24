cEpsNode__SWIG_1 <-
structure(function(dim, pa, pb, E1)
{
  dim = as.integer(dim); 
  
  if(length(dim) > 1) {
    warning("using only the first element of dim");
  };
  
  
  
  
  ;ans = .Call('R_swig_new_cEpsNode__SWIG_1', dim, pa, pb, E1, PACKAGE='peer');
  class(ans) <- "_p_PEER__cEpsNode";
  
  reg.finalizer(ans, delete_cEpsNode)
  ans
  
}
, returnType = "_p_PEER__cEpsNode", inputTypes = c("integer", 
"numeric", "numeric", "_p_PMatrix"), class = c("SWIGFunction", 
"character"))

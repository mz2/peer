logdet <-
structure(function(m, .copy = FALSE)
{
  ;.Call('R_swig_logdet', m, as.logical(.copy), PACKAGE='peer');
  
}
, returnType = "numeric", inputTypes = "_p_PMatrix", class = c("SWIGFunction", 
"character"))

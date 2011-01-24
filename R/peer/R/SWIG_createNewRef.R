SWIG_createNewRef <-
function(className, ..., append = TRUE)
{
  f = get(paste("new", className, sep = "_"), mode = "function")

  f(...)
}


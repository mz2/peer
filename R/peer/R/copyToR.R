copyToR <-
structure(function(value, obj = new(gsub("Ref$", "", class(value)))) 
               standardGeneric("copyToR"
           )
, generic = structure("copyToR", package = ".GlobalEnv"), package = ".GlobalEnv", group = list(), valueClass = character(0), signature = c("value", 
"obj"), default = quote(`NULL`), skeleton = quote(function (value, 
    obj = new(gsub("Ref$", "", class(value)))) 
stop("invalid call in method dispatch to \"copyToR\" (no default method)", 
    domain = NA)(value, obj)), class = structure("standardGeneric", package = "methods"))

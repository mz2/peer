defineEnumeration <-
function(name, .values, where = topenv(parent.frame()), suffix = "Value")
{
   # Mirror the class definitions via the E analogous to .__C__
  defName = paste(".__E__", name, sep = "")
  assign(defName,  .values,  envir = where)

  if(nchar(suffix))
    name = paste(name, suffix, sep = "")

  setClass(name, contains = "EnumerationValue", where = where)
}


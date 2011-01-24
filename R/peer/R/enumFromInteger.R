enumFromInteger <-
function(i,type)
{
  itemlist <- get(paste(".__E__", type, sep=""))
  names(itemlist)[match(i, itemlist)]
}


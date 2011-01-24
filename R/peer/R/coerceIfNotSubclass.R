coerceIfNotSubclass <-
function(obj, type) 
{
    if(!is(obj, type)) {as(obj, type)} else obj
}


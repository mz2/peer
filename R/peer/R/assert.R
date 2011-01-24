assert <-
function(condition, message = "")
{
  if(!condition)
    stop(message)

  TRUE
}


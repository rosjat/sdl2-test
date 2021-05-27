local automation = {}


automation.dynamics = {}

function automation.hi()
   print("Hi!")
end

--[[

This is something that smells, since it will only work if we get the c stuff into lua.
That said we only require the module after we created the stack and at that point the 
c function should be up and running. From a design point, its ugly and if I ever go more 
into lua i might try to figure out how to  make it work with somewhat clear chain of 
getting to the point where sdl2_test_lua comes from ...

]]--
function automation.AlphaAutomation(app, b)
    while true do
       sdl2_test_lua.SetAlpha(app, b, 0, 255, 5)
       coroutine.yield()
       sdl2_test_lua.Sleep(app, 60)
       coroutine.yield()
       sdl2_test_lua.SetAlpha(app, b, 255, 0, -5)
       coroutine.yield()
       sdl2_test_lua.Sleep(app, 30)
       coroutine.yield()
    end
 end
 
 function automation.IssueNextTask(app, b)
    if coroutine.status(automation.dynamics[b].behaviour, app, b) ~= "dead" then
       coroutine.resume(automation.dynamics[b].behaviour, app, b)
    end
 end

 function automation.AddAutomationToBlock(block, routine)
    automation.dynamics[block] = { behaviour = coroutine.create(routine)}
 end

 return automation
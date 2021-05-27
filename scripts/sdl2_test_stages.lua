--[[
   to keep the lua function somewhat out of the scope of the lua scripts,
   we have a c function that executes a string holding our functions.
   This isn't flexible regarding changes in the functionality but this isn't
   the aim for these scripts! Here we want to assemble information for one or 
   more stages. 

   Note: at the moment the next lines till stages = {} have no impact on the 
         stage creation!

         For now it simply reads a file and extract a tile info (solid/enter) 
         and sets a matrix of 16 columns and 15 rows that translate to 512px by
         480px and a tile size of 32px by 32px. 

   In the next iteration the functions InitAllStages and ReloadStage should also move 
   to the hardcoded part. Another stage would mostlikely have the same setup for maping
   the screens.
   
]]--
sdl2_test_lua.ImportFunctions()

automation = require('scripts.sdl2_test_automation')


-- we need this as a baseline for our lvl script reload
local lvl_config_path = 'scripts/sdl2_test_lvl.config'
local lvl_base_line = read_entire_file_text(lvl_config_path)

function InitAllStages()
   local lvl_block_table = load_levels(lvl_config_path)
   local trect = 0
   local brect = 1

   local stage = sdl2_test_lua.InitStage(23, 0)
   local x = 10
   local y = 4788
   sdl2_test_lua.InitScreen(stage, 0, x , y, 512, 480, 1)
   for s = 1, 6 , 1 do
      sdl2_test_lua.InitScreen(stage, s, x , y - (s  * 240), 512, 480, 3)
   end
   sdl2_test_lua.InitScreen(stage, 7, x , y - (7 * 240), 512, 480, 10)
   x = x +256
   sdl2_test_lua.InitScreen(stage, 8, x , y - (7 * 240), 512, 480, 5)
   for s = 9, 15 , 1 do
      sdl2_test_lua.InitScreen(stage, s, x , y - ((s - 1) * 240), 512, 480, 3)
   end
   sdl2_test_lua.InitScreen(stage, 16, x , y - (15 * 240), 512, 480, 10)
   x = x + 256
   sdl2_test_lua.InitScreen(stage, 17, x , y - (15 * 240), 512, 480, 5)
   sdl2_test_lua.InitScreen(stage, 18, x , y - (16 * 240), 512, 480, 3)
   sdl2_test_lua.InitScreen(stage, 19, x , y - (17 * 240), 512, 480, 10)
   x = x + 256 
   sdl2_test_lua.InitScreen(stage, 20, x , y - (17 * 240), 512, 480, 5)
   for s = 21, 22 , 1 do
      sdl2_test_lua.InitScreen(stage, s, x , y - ((s- 3) * 240), 512, 480, 3)
   end
   sdl2_test_lua.InitScreen(stage, 23, x , y - (20 * 240), 512, 480, 2)

   for k,v in pairs(lvl_block_table) do
      local row = 0
      local col = 0
      local index = 0;
      for k2,v2 in pairs(v) do
            local bid = k2 -1
            local solid = v2["solid"]
            local enter = v2["enter"]
            sdl2_test_lua.InitBlock(stage, (k -1), index, enter, solid)
            sdl2_test_lua.InitRect(stage, (k -1), index, trect, 0, 0, 1, 1)
            sdl2_test_lua.InitRect(stage, (k -1), index, brect, col * 32 , row * 32, 32, 32)
            if(solid == 2) then
               local b = sdl2_test_lua.GetBlock(stage, (k -1), index)
               print(b)
               automation.dynamics[b] = { behaviour = coroutine.create(automation.AlphaAutomation)}
            end
            col = (k2) % 16
            if (col == 0) and k2 > 15 then
               row = row + 1
            end 
            if solid >= 1 then
               index = index +1
            end
      end
   end
   table.insert(stages,  1 ,stage)
end

--[[

   TODO: this is broken again !!!
   
         Since we moved away from only solid and not solid we need to put in some
         more work in here.

         - how to handle blocks that where there but are gone
         - how to handle blocks that are gone and hat automation
         - how to handle blocks that are new with/without automation

]]--    
function ReloadStage(stg)
   if false then
      print("reload enter", stg)
      local lvls  = read_entire_file_text(lvl_config_path)
      if lvls ~= lvl_base_line then
         print("reload changes", stg)
         local lvl_table = load_levels(lvl_config_path)
         local trect = 0
         local brect = 1
         local s = stages[stg]
         for k,v in pairs(lvl_table) do
            local row = 0
            local col = 0
            for k2,v2 in pairs(v) do
                  local bid = k2 -1
                  local solid = v2["solid"]
                  local enter = v2["enter"]
                  sdl2_test_lua.InitBlock(s, (k -1), bid, enter, solid)
                  print(solid, enter)
                  if solid == 1 and enter == 1 then
                     sdl2_test_lua.SetRect(s, (k -1), bid, trect, 15, 15, 1, 1)
                  else
                     sdl2_test_lua.SetRect(s, (k -1), bid, trect, 0, 0, 2, 2)
                  end
                  sdl2_test_lua.SetRect(s, (k -1), bid, brect, col * 32 , row * 32, 32, 32)
                  col = (k2) % 16
                  if (col == 0) and k2 > 15 then
                     row = row + 1
                  end 
            end
         end
         lvl_base_line = lvls
      end
      print("reload exit")
   else
      print("reload is disabled for now ...")
   end
end

stages = {}

InitAllStages()


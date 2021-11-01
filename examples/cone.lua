function create_cone(plist)
   local position1 = {0, 0, 0}
   local radius1 = 100
   
   local position2 = {0, 0, 250}
   local radius2 = 10
   
   local shape = cone(position1, position2, radius1, radius2)
   local num_samples = 500
   sample_shape(plist, shape, num_samples)
end

-- create the particle list
local plist = create_particle_list()

-- create a "cone"
create_cone(plist)
   
-- render the spheres into an SDF
local grid = create_grid(2.0)
particles_to_sdf(plist, grid)

-- generate the output file
write_obj("output.obj", grid)

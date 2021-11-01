-- creates a 3D grid of 10 x 10 x 10 spheres
function sphere_grid(plist)
   -- create the 3D grid of spheres
   local offset = 18
   local radius = 10
   for x = 1, 10 do
      for y = 1, 10 do
	 for z = 1, 10 do
	    add_particle(plist, {offset*x, offset*y, offset*z}, radius)
	 end
      end
   end
end

-- create the particle list
local plist = create_particle_list()

-- create a grid of spheres
sphere_grid(plist)

-- render the spheres into an SDF
local grid = create_grid(2.0)
particles_to_sdf(plist, grid)

-- generate the output file
write_obj("output.obj", grid)

project('jody', 'c', default_options : ['c_std=c11'], version : '0.1.0', license : 'GPL2+')

add_global_arguments('-DVERSION="0.1.0"', language : 'c')

sdl = dependency('sdl3', version: '>=2')
sdlimage = dependency('sdl3-image', version: '>=2')

subdir('src')


#install_subdir('mydir', install_dir : 'include') # mydir subtree -> include/mydir
#install_data('datafile.cat', install_dir : join_paths(get_option('datadir'), 'progname')) # -> share/progname/datafile.dat

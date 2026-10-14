exe = 'jody'

project_source_root = meson.project_source_root()
project_build_root = meson.project_build_root()

source_img_dir = join_paths(project_source_root, 'src', 'img')

install_img_dir = join_paths(
  get_option('prefix'),
  get_option('datadir'),
  exe,
  'img'
)

datadir_conf = configuration_data()
datadir_conf.set('SOURCE_IMG_DIR', source_img_dir)
datadir_conf.set('INSTALL_IMG_DIR', install_img_dir)

datadir_conf.set('IMG_DIR', source_img_dir + '/')

generated_datadir_h = configure_file(
  input: 'datadir.h.in',
  output: 'datadir.h',
  configuration: datadir_conf
)

inc = include_directories('.')

sources = [
    'brands.c',
    'brands.h',
    'cursor.c',
    'cursor.h',
    'gamedefs.h',
    'main.c',
    'sprite.h',
    'sprite.c',
    'surface.c',
    'surface.h',
    'util.c',
    'util.h',
    'wheelhouse.c',
    'wheelhouse.h',
    generated_datadir_h
]
executable(
    exe,
    sources,
    install: true,
    dependencies: [sdl, sdlimage]
)

imgdir = join_paths(get_option('datadir'), exe, 'img') # share/jody/img
message(imgdir)

install_data(
  'img/icon.png',
  'img/wheelhouse_image_black_with_white_lines.png',
  'img/wheelhouse_image_inverted.png',
  'img/wheelhouse_image_transparent.png',
  install_dir : imgdir
)

install_subdir('img/background', install_dir : imgdir)
install_subdir('img/cloud', install_dir : imgdir)
install_subdir('img/cow_black', install_dir : imgdir)
install_subdir('img/cow_brown', install_dir : imgdir)
install_subdir('img/cow_orange', install_dir : imgdir)
install_subdir('img/cow_red', install_dir : imgdir)
install_subdir('img/cow_white', install_dir : imgdir)
install_subdir('img/cow_yellow', install_dir : imgdir)
install_subdir('img/cow_yellow_with_red_outline', install_dir : imgdir)
install_subdir('img/kangaroo_black', install_dir : imgdir)
install_subdir('img/kangaroo_grey', install_dir : imgdir)
install_subdir('img/kangaroo_red', install_dir : imgdir)

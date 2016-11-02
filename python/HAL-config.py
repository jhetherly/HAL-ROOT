import os
import argparse
import pickle


def write_settings(hal_settings, config):
    pickle.dump(config, open(hal_settings, "wb"))


def main(hal_settings, arg_options):
    config = pickle.load(open(hal_settings, "rb"))
    print_string = ""

    for key in arg_options:
        if arg_options[key]:
            print_string += ' '+config[key]

    if print_string is not "":
        print print_string[1:]

if __name__ == "__main__":
    absolute_real_path_to_HAL_source = os.path.dirname(os.path.realpath(__file__))
    settings_pickle_file = os.path.join(absolute_real_path_to_HAL_source, "settings.pkl")

    parser = argparse.ArgumentParser(description='Configuration settings for HAL-ROOT')

    parser.add_argument('--installdir', default=False, action='store_const', const=True, help='print the directory of the installation')
    parser.add_argument('--libdir', default=False, action='store_const', const=True, help='print the directory of the library')
    parser.add_argument('--incdir', default=False, action='store_const', const=True, help='print the directory of the header files')
    parser.add_argument('--libs', default=False, action='store_const', const=True, help='print the library to link to for HAL')
    parser.add_argument('-w', '--write', nargs='*', help='give settings to be stored')

    args = parser.parse_args()

    if args.write:
        config = {}
        config['installdir'] = args.write[0]
        config['libdir'] = args.write[1]
        config['incdir'] = args.write[2]
        config['libs'] = '-lHAL'
        write_settings(settings_pickle_file, config)

    d = args.__dict__
    try:
        del d['write']
    except KeyError:
        pass
    main(settings_pickle_file, d)

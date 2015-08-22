from distutils.core import setup, Extension

module1 = Extension(
        'pdfmax',
        sources = [
                'pdfmaxmodule.c',
                'pdfMax.c' ],
        extra_compile_args = ['-std=c99', '-pedantic']
)

setup(
        name         = 'pdfmax',
        version      = '1.0a',
        description  = 'Fast maximum of multiple Gaussians',
        author       = 'Marcel van Kervinck',
        author_email = 'marcelk@bitpit.net',
        url          = 'http://marcelk.net/',
        ext_modules  = [module1])

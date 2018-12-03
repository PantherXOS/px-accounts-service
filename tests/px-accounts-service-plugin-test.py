import pkgutil

if pkgutil.find_loader('PluginFramework') is not None:

    import PluginFramework

    class Plugin(PluginFramework.Plugin):       # important: class name need to be named as `Plugin`

        title = 'sample_service'

        def __init__(self):
            pass

        def verify(self, params):
            pdict = { 'k1': 'v1', 'k2': 'v2' }
            result = PluginFramework.VerifyResult()
            result.verified = True

            klist = []
            for k in params:
                klist.append(k)

            for k in pdict.keys():
                if k not in klist:
                    result.verified = False
                    result.errors.append('{} is not in params'.format(k))
                elif params[k] != pdict[k]:
                    result.verified = False
                    result.errors.append('{} value is invalid {} != {}'.format(k, params[k], pdict[k]))
            return result

        def authenticate(self, params):
            result = PluginFramework.AuthResult()
            result.authenticated = False
            result.errors.append('method not implemented.')
            return  result
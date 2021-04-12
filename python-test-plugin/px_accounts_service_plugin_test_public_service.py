import pkgutil
import importlib

if pkgutil.find_loader('PluginFramework') is not None:
    PluginFramework = importlib.import_module('PluginFramework')

    class Plugin(PluginFramework.Plugin):

        title = 'public-test'

        def __init__(self):
            PluginFramework.Plugin.__init__(self)
            pass

        def verify(self, params):
            result = PluginFramework.VerifyResult()
            result.verified = True
            return result

        def authenticate(self, params):
            result = PluginFramework.AuthResult()
            result.authenticated = True
            return result

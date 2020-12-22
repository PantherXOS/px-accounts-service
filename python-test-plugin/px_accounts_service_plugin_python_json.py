import pkgutil
import importlib
import json
import os
import  uuid

if pkgutil.find_loader('PluginFramework') is not None:
    PluginFramework = importlib.import_module('PluginFramework')

    class Plugin(PluginFramework.Plugin):

        title = 'python-json'

        def __init__(self):
            pass

        def verify(self, params):
            required_params  = [ 'k2' ]
            protected_params = [ 'k1' ]
            optional_params  = { 'o1': 'ov1', 'o2': 'ov2' }
            result = PluginFramework.VerifyResult()
            result.verified = True

            received_keys = []
            for k in params:
                received_keys.append(k)

            for k in required_params:
                if not k in received_keys:
                    result.verified = False
                    result.errors.append('Param \'{}\' is required'.format(k))

            if result.verified:
                for k in received_keys:
                    param = PluginFramework.ServiceParam()
                    param.key = k
                    param.val = params[k]
                    if k in required_params:
                        param.is_required = True
                    if k in protected_params:
                        param.is_protected = True
                    result.params.append(param)

                for k in protected_params:
                    if k not in received_keys:
                        param = PluginFramework.ServiceParam()
                        param.key = k
                        param.is_protected = True
                        result.params.append(param)

                for k in optional_params.keys():
                    if k not in received_keys:
                        param = PluginFramework.ServiceParam()
                        param.key = k
                        param.val = optional_params[k]
                        param.default_val = optional_params[k]
                        if k in protected_params:
                            param.is_protected = True
                        result.params.append(param)
            return result

        def authenticate(self, params):
            result = PluginFramework.AuthResult()
            token = PluginFramework.SecretToken()
            token.label = 't1'
            token.secret = 'token_value1'
            result.tokens.append(token)
            result.authenticated = True
            result.errors.append('sample warning')
            return result

        def read(self, id):
            result = PluginFramework.StrStrMap()
            with open("{}.json".format(id)) as plugin_file:
                data = json.load(plugin_file)
                for key in data:
                    result[key] = data[key]
            return result

        def write(self, vResult, aResult):
            id = str(uuid.uuid4())
            data = {}
            for param in vResult.params:
                data[param.key] = param.val
            with open('{}.json'.format(id), 'w') as plugin_file:
                json.dump(data, plugin_file)
            return id

        def remove(self, id):
            plugin_file_name = '{}.json'.format(id)
            if os.path.exists(plugin_file_name):
                os.remove(plugin_file_name)
                return True
            return False


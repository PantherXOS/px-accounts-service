//
// Created by Reza Alizadeh Majd on 2019-08-14.
//

#ifndef PX_ACCOUNTS_SERVICE_SECRETSIMULATOR_H
#define PX_ACCOUNTS_SERVICE_SECRETSIMULATOR_H

#include <interface/Secret.capnp.h>

#include <iostream>
#include <list>
#include <map>
#include <string>
#include <vector>

using namespace std;

struct MockSecret {
    string label;
    map<string, string> attributes;
    map<string, string> secrets;
};

class SecretSimulator final : public RPCSecretService::Server {
   private:
    list<MockSecret> _secrets;
    map<string, list<string> > _schemas;
    list<string> _attributes;

   protected:
    kj::Promise<void> getSupportedAttributes(GetSupportedAttributesContext context) override;
    kj::Promise<void> getSupportedSchemas(GetSupportedSchemasContext context) override;
    kj::Promise<void> getSchemaKeys(GetSchemaKeysContext context) override;
    kj::Promise<void> setSecret(SetSecretContext context) override;
    kj::Promise<void> search(SearchContext context) override;
    kj::Promise<void> deleteSecret(DeleteSecretContext context) override;

   public:
    explicit SecretSimulator();
};

#endif  // PX_ACCOUNTS_SERVICE_SECRETSIMULATOR_H

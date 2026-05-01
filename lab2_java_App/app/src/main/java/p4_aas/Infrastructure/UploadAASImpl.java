// Copyright 2023 riccardo.bacca@studio.unibo.it
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
//     http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

package p4_aas.Infrastructure;

import org.eclipse.basyx.aas.metamodel.map.AssetAdministrationShell;
import org.eclipse.basyx.aas.metamodel.map.descriptor.AASDescriptor;
import org.eclipse.basyx.aas.metamodel.map.descriptor.SubmodelDescriptor;
import org.eclipse.basyx.aas.registration.proxy.AASRegistryProxy;
import org.eclipse.basyx.submodel.metamodel.map.Submodel;

import java.util.List;

public class UploadAASImpl implements UploadAAS {
    private String REGISTRYPATH;
    private AASRegistryProxy registryProxy;

    public UploadAASImpl(String REGISTRYPATH) {
        this.REGISTRYPATH = REGISTRYPATH;
        this.registryProxy = new AASRegistryProxy(this.REGISTRYPATH);
    }

    @Override
    public void registerAASSMDescriptors(Integer PORT, List<Submodel> submodels, AssetAdministrationShell shell){
        String IP = this.generateAASIP(PORT);
        AASDescriptor aasDescriptor = new AASDescriptor(shell, IP);
        
        for (Submodel sm : submodels) {
            aasDescriptor.addSubmodelDescriptor(new SubmodelDescriptor(sm, IP + "/submodels/" + sm.getIdShort()));
        }
        
        this.registryProxy.register(aasDescriptor);
    }

    private String generateAASIP(Integer port) {
        return String.format("http://localhost:%d/aas", port);
    }
}
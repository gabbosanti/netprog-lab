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

package p4_aas.AssetShells;

import java.util.LinkedList;
import java.util.List;

import org.eclipse.basyx.aas.metamodel.api.parts.asset.AssetKind;
import org.eclipse.basyx.aas.metamodel.map.AssetAdministrationShell;
import org.eclipse.basyx.aas.metamodel.map.parts.Asset;
import org.eclipse.basyx.submodel.metamodel.api.identifier.IIdentifier;
import org.eclipse.basyx.submodel.metamodel.api.identifier.IdentifierType;
import org.eclipse.basyx.submodel.metamodel.map.Submodel;
import org.eclipse.basyx.submodel.metamodel.map.identifier.Identifier;

import p4_aas.StaticProperties;
import p4_aas.Infrastructure.ModelProvider;
import p4_aas.Infrastructure.UploadAAS;
import p4_aas.Infrastructure.UploadAASImpl;

/**
 * Abstract class for Asset Administration Shells
 * Implements IShell
 */
public abstract class AbstractShell implements IShell {
    private final String ACCESS_CONTROL_ALLOW_ORIGIN = "*";
    private final String CONTEXT_PATH = "/";

    protected AssetAdministrationShell shell;
    protected List<Submodel> submodels;
    protected ModelProvider modelProvider;
    protected UploadAAS descriptorsManager;
    protected final int PORT;

    public AbstractShell(int PORT) {
        this.PORT = PORT;
        this.submodels = new LinkedList<>();
    }

    @Override
    public void createAndStartServlet() {
        this.addSubmodels();
        this.createModelProvider();
        this.createDescriptors();
    }

    @Override
    public int getPORT() {
        return PORT;
    }

    @Override
    public AssetAdministrationShell getShell() {
        return shell;
    }

    @Override
    public ModelProvider getModelProvider() {
        return modelProvider;
    }

    @Override
    public List<Submodel> getSubmodels() {
        return submodels;
    }

    @Override
    public void createDescriptors() {
        this.descriptorsManager = new UploadAASImpl(StaticProperties.REGISTRYPATH);
        this.descriptorsManager.registerAASSMDescriptors(this.PORT, this.submodels, this.shell);
    }

    @Override
    public void addSubmodels() {
        this.submodels.forEach(el -> this.shell.addSubmodel(el));
    }

    @Override
    public AssetAdministrationShell createShell(String idShort, String pathId, AssetKind kind) {
        IIdentifier id = new Identifier(IdentifierType.IRI, pathId);
        Asset ovsAsset = new Asset(idShort, id, kind);
        
        return new AssetAdministrationShell(idShort, id, ovsAsset);
    }

    @Override
    public void createModelProvider() {
        this.modelProvider = new ModelProvider(
            this.PORT, 
            this.ACCESS_CONTROL_ALLOW_ORIGIN, 
            this.CONTEXT_PATH,
            this.shell, 
            this.submodels);
        this.modelProvider.startLocalHTTPServer(shell.getIdShort());
    }
}

package com.matterhub.server.entities.matter.generated.onoff;

import java.util.List;
import java.util.Optional;

import org.eclipse.ditto.json.JsonField;

import com.matterhub.server.entities.matter.Attribute;
import com.matterhub.server.entities.matter.Cluster;
import com.matterhub.server.entities.matter.Command;

public class OnOffCluster extends Cluster {

    private List<Attribute> attributes;
    private List<Command> commands;

    public OnOffCluster(List<Integer> attributeIds, List<Integer> commandIds, Integer filler) {
        this.attributes = attributeIds.stream().map(this::getAttribute).toList();
        // TODO assert that it contains the required attributes
        this.commands = commandIds.stream().map(this::getCommand).toList();
        // TODO assert that it contains the required commands
    }

    public OnOffCluster(List<String> attributeNames, List<String> commandNames, String filler) {
        this.attributes = attributeNames.stream().map(this::getAttribute).toList();
        // TODO assert that it contains the required attributes
        this.commands = commandNames.stream().map(this::getCommand).toList();
        // TODO assert that it contains the required commands
    }

    @Override
    public int Id() {
        return 6;
    }

    @Override
    public String Name() {
        return "On/Off";
    }

    @Override
    public List<Attribute> Attributes() {
        return attributes;
    }

    @Override
    public List<Command> Commands() {
        return commands;
    }
    // TODO create the required attributes and commands

    @Override
    public Attribute getAttribute(int id) {
        switch (id) {
            case 0: {
                OnOffAttribute attr = new OnOffAttribute();
                attr.parentCluster = this;
                return attr;
            }

        }
        throw new IllegalArgumentException("Unkown attribute id: " + id);
    }

    @Override
    public Attribute getAttribute(String name) {
        switch (name) {
            case "OnOff": {
                OnOffAttribute attr = new OnOffAttribute();
                attr.parentCluster = this;
                return attr;
            }

        }
        throw new IllegalArgumentException("Unkown attribute id: " + name);
    }

    @Override
    public Command getCommand(int id) {
        throw new UnsupportedOperationException("Commands aren't yet handled");
    }

    @Override
    public Command getCommand(String name) {
        throw new UnsupportedOperationException("Commands aren't yet handled");
    }
}

class OnOffAttribute implements Attribute {
    Cluster parentCluster;
    Optional<Boolean> state;

    @Override
    public int Id() {
        return 0;
    }

    @Override
    public String Name() {
        return "OnOff";
    }

    @Override
    public Cluster Parent() {
        return this.parentCluster;
    }

    @Override
    public byte[] toMatterValue() {
        // TODO Auto-generated method stub
        return null;
    }

    @Override
    public void fromMatterValue(byte[] matterValue) {
        // TODO Auto-generated method stub
    }

    @Override
    public JsonField toThingsRepresentation() {
        // TODO Auto-generated method stub
        return null;
    }

    @Override
    public void fromThingsRepresentation(JsonField featureValue) {
        // TODO Auto-generated method stub

    }

}

package com.matterhub.server.entities.matter.generated.onoff;

import java.util.List;

import com.matterhub.server.entities.matter.Attribute;
import com.matterhub.server.entities.matter.Cluster;
import com.matterhub.server.entities.matter.Command;

public class OnOffCluster extends Cluster {

    private List<Attribute> attributes;
    private List<Command> commands;

    public OnOffCluster(List<Integer> attributeIds, List<Integer> commandIds) {
        this.attributes = attributes;
        // TODO assert that it contains the required attributes
        this.commands = commands;
        // TODO assert that it contains the required commands
    }

    @Override
    public int Id() {
        return 6;
    }

    @Override
    public String Name() {
        return "on-off";
    }

    @Override
    public List<Attribute> Attributes() {
        // TODO Auto-generated method stub
        return null;
    }

    @Override
    public List<Command> Commands() {
        // TODO Auto-generated method stub
        return null;
    }

}

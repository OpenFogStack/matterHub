package com.matterhub.server.entities.matter.generated.onoff;

import com.matterhub.server.entities.Metric;
import com.matterhub.server.entities.matter.Attribute;
import com.matterhub.server.entities.matter.Cluster;
import com.matterhub.server.entities.matter.Command;
import org.eclipse.ditto.json.JsonField;
import org.eclipse.ditto.json.JsonValue;

import java.util.List;
import java.util.Objects;

enum StartUpOnOff {
    Off(0x00), On(0x01), TogglePreviousOnOff(0x02);
    final byte value;

    StartUpOnOff(int value) {
        this.value = (byte) value;
    }

    static StartUpOnOff fromValue(int value) {
        for (StartUpOnOff variant : StartUpOnOff.values()) {
            if (variant.value == value) {
                return variant;
            }
        }
        throw new IllegalArgumentException("No such enum variant");
    }
}

enum OnOffEffectIdentifier {
    DelayedAllOff(0x00), DyingLight(0x01);
    final byte value;

    OnOffEffectIdentifier(int value) {
        this.value = (byte) value;
    }

    static OnOffEffectIdentifier fromValue(int value) {
        for (OnOffEffectIdentifier variant : OnOffEffectIdentifier.values()) {
            if (variant.value == value) {
                return variant;
            }
        }
        throw new IllegalArgumentException("No such enum variant");
    }
}

enum OnOffDelayedAllOffEffectVariant {
    DelayedAllOff(0x00), DyingLight(0x01);
    final byte value;

    OnOffDelayedAllOffEffectVariant(int value) {
        this.value = (byte) value;
    }

    static OnOffDelayedAllOffEffectVariant fromValue(int value) {
        for (OnOffDelayedAllOffEffectVariant variant : OnOffDelayedAllOffEffectVariant.values()) {
            if (variant.value == value) {
                return variant;
            }
        }
        throw new IllegalArgumentException("No such enum variant");
    }
}

public class OnOffCluster extends Cluster {

    private final List<Attribute> attributes;
    private final List<Command> commands;

    public OnOffCluster(List<Integer> attributeIds, List<Integer> commandIds, Integer filler) {
        this.attributes = attributeIds.stream().map(this::getAttribute).filter(Objects::nonNull).toList();
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

    @Override
    public Attribute getAttribute(int id) {
        switch (id) {
            case 0x0000: {
                var attr = new OnOffAttribute();
                attr.parentCluster = this;
                return attr;
            }
            case 0x4000: {
                var attr = new GlobalSceneControlAttribute();
                attr.parentCluster = this;
                return attr;
            }
            case 0x4001: {
                var attr = new OnTimeAttribute();
                attr.parentCluster = this;
                return attr;
            }
            case 0x4002: {
                var attr = new OffWaitTimeAttribute();
                attr.parentCluster = this;
                return attr;
            }
            case 0x4003: {
                var attr = new StartUpOnOffAttribute();
                attr.parentCluster = this;
                return attr;
            }
        }
        if (0XFFF8 <= id && id <= 0XFFFD) {
            System.out.println("Ignoring global attributes");
            return null;
        }
        throw new IllegalArgumentException("Unknown attribute id: " + id);
    }

    @Override
    public Attribute getAttribute(String name) {
        switch (name) {
            case "OnOff": {
                var attr = new OnOffAttribute();
                attr.parentCluster = this;
                return attr;
            }
            case "GlobalSceneControl": {
                var attr = new GlobalSceneControlAttribute();
                attr.parentCluster = this;
                return attr;
            }
            case "OnTime": {
                var attr = new OnTimeAttribute();
                attr.parentCluster = this;
                return attr;
            }
            case "OffWaitTime": {
                var attr = new OffWaitTimeAttribute();
                attr.parentCluster = this;
                return attr;
            }
            case "StartUpOnOff": {
                var attr = new StartUpOnOffAttribute();
                attr.parentCluster = this;
                return attr;
            }

        }
        throw new IllegalArgumentException("Unknown attribute id: " + name);
    }

    @Override
    public Command getCommand(int id) {
        switch (id) {
            case 0x00: {
                var cmd = new OffCommand();
                cmd.parentCluster = this;
                return cmd;
            }
            case 0x01: {
                var cmd = new OnCommand();
                cmd.parentCluster = this;
                return cmd;
            }
            case 0x02: {
                var cmd = new ToggleCommand();
                cmd.parentCluster = this;
                return cmd;
            }
            case 0x40: {
                var cmd = new OffWithEffectCommand();
                cmd.parentCluster = this;
                return cmd;
            }
            case 0x41: {
                var cmd = new OnWithRecallGlobalSceneCommand();
                cmd.parentCluster = this;
                return cmd;
            }
            case 0x42: {
                var cmd = new OnWithTimedOffCommand();
                cmd.parentCluster = this;
                return cmd;
            }

        }
        throw new IllegalArgumentException("Unknown attribute id: " + id);
    }

    @Override
    public Command getCommand(String name) {
        switch (name) {
            case "Off": {
                var cmd = new OffCommand();
                cmd.parentCluster = this;
                return cmd;
            }
            case "On": {
                var cmd = new OnCommand();
                cmd.parentCluster = this;
                return cmd;
            }
            case "Toggle": {
                var cmd = new ToggleCommand();
                cmd.parentCluster = this;
                return cmd;
            }
        }
        throw new IllegalArgumentException("Unknown attribute id: " + name);
    }
}

class OnOffAttribute implements Attribute {

    boolean state;
    Cluster parentCluster;

    @Override
    public int Id() {
        return 0x0000;
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
    public Metric toMetric() {
        // TODO Auto-generated method stub
        return null;
    }

    @Override
    public void fromMatterValue(byte[] matterValue) {
        // TODO Auto-generated method stub
    }

    @Override
    public JsonField toThingsRepresentation() {
        return JsonField.newInstance(Name(), JsonValue.of(state));
    }

    @Override
    public void fromThingsRepresentation(JsonField featureValue) {
        assert this.Name().contentEquals(featureValue.getKey());
        this.state = featureValue.getValue().asBoolean();
    }
}

class GlobalSceneControlAttribute implements Attribute {
    Cluster parentCluster;
    boolean state;

    @Override
    public int Id() {
        return 0x4000;
    }

    @Override
    public String Name() {
        return "GlobalSceneControl";
    }

    @Override
    public Cluster Parent() {
        return this.parentCluster;
    }

    @Override
    public Metric toMetric() {
        // TODO Auto-generated method stub
        return null;
    }

    @Override
    public void fromMatterValue(byte[] matterValue) {
        // TODO Auto-generated method stub
    }

    @Override
    public JsonField toThingsRepresentation() {
        return JsonField.newInstance(Name(), JsonValue.of(state));
    }

    @Override
    public void fromThingsRepresentation(JsonField featureValue) {
        assert this.Name().contentEquals(featureValue.getKey());
        this.state = featureValue.getValue().asBoolean();
    }

}

class OnTimeAttribute implements Attribute {
    Cluster parentCluster;
    short state;

    @Override
    public int Id() {
        return 0x4001;
    }

    @Override
    public String Name() {
        return "OnTime";
    }

    @Override
    public Cluster Parent() {
        return this.parentCluster;
    }

    @Override
    public Metric toMetric() {
        // TODO Auto-generated method stub

        //Writable true
        return null;
    }

    @Override
    public void fromMatterValue(byte[] matterValue) {
        // TODO Auto-generated method stub
    }

    @Override
    public JsonField toThingsRepresentation() {
        return JsonField.newInstance(Name(), JsonValue.of(state));
    }

    @Override
    public void fromThingsRepresentation(JsonField featureValue) {
        assert this.Name().contentEquals(featureValue.getKey());
        this.state = (short) featureValue.getValue().asInt();
    }

}

class OffWaitTimeAttribute implements Attribute {
    Cluster parentCluster;
    short state;

    @Override
    public int Id() {
        return 0x4002;
    }

    @Override
    public String Name() {
        return "OffWaitTime";
    }

    @Override
    public Cluster Parent() {
        return this.parentCluster;
    }

    @Override
    public Metric toMetric() {
        // TODO Auto-generated method stub
        //Writable true

        return null;
    }

    @Override
    public void fromMatterValue(byte[] matterValue) {
        // TODO Auto-generated method stub
    }

    @Override
    public JsonField toThingsRepresentation() {
        return JsonField.newInstance(Name(), JsonValue.of(state));
    }

    @Override
    public void fromThingsRepresentation(JsonField featureValue) {
        assert this.Name().contentEquals(featureValue.getKey());
        this.state = (short) featureValue.getValue().asInt();
    }

}

class StartUpOnOffAttribute implements Attribute {
    Cluster parentCluster;
    StartUpOnOff state;

    @Override
    public int Id() {
        return 0x4003;
    }

    @Override
    public String Name() {
        return "StartUpOnOff";
    }

    @Override
    public Cluster Parent() {
        return this.parentCluster;
    }

    @Override
    public Metric toMetric() {
        // TODO Auto-generated method stub
        return null;
    }

    @Override
    public void fromMatterValue(byte[] matterValue) {
        // TODO Auto-generated method stub
    }

    @Override
    public JsonField toThingsRepresentation() {
        return JsonField.newInstance(Name(), JsonValue.of(state));
    }

    @Override
    public void fromThingsRepresentation(JsonField featureValue) {
        assert this.Name().contentEquals(featureValue.getKey());
        this.state = StartUpOnOff.fromValue(featureValue.getValue().asInt());
    }
}

class OffCommand implements Command {
    Cluster parentCluster;

    @Override
    public int Id() {
        return 0x00;
    }

    @Override
    public String Name() {
        return "Off";
    }

    @Override
    public Cluster Parent() {
        return this.parentCluster;
    }

    @Override
    public Metric toMetric() {
        return null;
    }
}

class OnCommand implements Command {
    Cluster parentCluster;

    @Override
    public int Id() {
        return 0x01;
    }

    @Override
    public String Name() {
        return "On";
    }

    @Override
    public Cluster Parent() {
        return this.parentCluster;
    }

    @Override
    public Metric toMetric() {
        return null;
    }
}

class ToggleCommand implements Command {
    Cluster parentCluster;

    @Override
    public int Id() {
        return 0x02;
    }

    @Override
    public String Name() {
        return "Toggle";
    }

    @Override
    public Cluster Parent() {
        return this.parentCluster;
    }

    @Override
    public Metric toMetric() {
        return null;
    }
}

class OffWithEffectCommand implements Command {
    Cluster parentCluster;
    OnOffEffectIdentifier effectId;
    OnOffDelayedAllOffEffectVariant effectVariant;
    OffWithEffectCommand() {}
    OffWithEffectCommand(OnOffEffectIdentifier effectId, OnOffDelayedAllOffEffectVariant effectVariant) {
        this.effectId = effectId;
        this.effectVariant = effectVariant;
    }

    @Override
    public int Id() {
        return 0x40;
    }

    @Override
    public String Name() {
        return "OffWithEffect";
    }

    @Override
    public Cluster Parent() {
        return this.parentCluster;
    }

    @Override
    public Metric toMetric() {
        return null;
    }
}

//TODO create constructor
class OnWithRecallGlobalSceneCommand implements Command {
    Cluster parentCluster;

    @Override
    public int Id() {
        return 0x41;
    }

    @Override
    public String Name() {
        return "OnWithRecallGlobalScene";
    }

    @Override
    public Cluster Parent() {
        return this.parentCluster;
    }

    @Override
    public Metric toMetric() {
        return null;
    }
}

//TODO create constructor
class OnWithTimedOffCommand implements Command {
    Cluster parentCluster;

    @Override
    public int Id() {
        return 0;
    }

    @Override
    public String Name() {
        return null;
    }

    @Override
    public Cluster Parent() {
        return this.parentCluster;
    }

    @Override
    public Metric toMetric() {
        return null;
    }
}

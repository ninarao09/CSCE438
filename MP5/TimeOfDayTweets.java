import java.io.IOException;
import java.util.StringTokenizer;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.IntWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.Mapper;
import org.apache.hadoop.mapreduce.Reducer;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;

public class TimeOfDayTweets {

  public static class TokenizerMapper extends Mapper<Object, Text, Text, IntWritable>{

    private final static IntWritable one = new IntWritable(1);
    private Text word = new Text();
	//private Text prev = new Text();
	//private Text prev2 = new Text();
	

    public void map(Object key, Text value, Context context) throws IOException, InterruptedException {
      StringTokenizer itr = new StringTokenizer(value.toString());
      while (itr.hasMoreTokens()) {
		//prev2 = prev;
		//prev = word;
		word.set(itr.nextToken());
		//if(prev2.toString().equals("T")){
			if(word.toString().startsWith("00:")){
				Text word2 = new Text(word.toString().substring(0,3));
				context.write(word2, one);
			}else if(word.toString().startsWith("01:")){
				Text word2 = new Text(word.toString().substring(0,3));
				context.write(word2, one);
			}else if(word.toString().startsWith("02:")){
				Text word2 = new Text(word.toString().substring(0,3));
				context.write(word2, one);
			}else if(word.toString().startsWith("03:")){
				Text word2 = new Text(word.toString().substring(0,3));
				context.write(word2, one);
			}else if(word.toString().startsWith("04:")){
				Text word2 = new Text(word.toString().substring(0,3));
				context.write(word2, one);
			}else if(word.toString().startsWith("05:")){
				Text word2 = new Text(word.toString().substring(0,3));
				context.write(word2, one);
			}else if(word.toString().startsWith("06:")){
				Text word2 = new Text(word.toString().substring(0,3));
				context.write(word2, one);
			}else if(word.toString().startsWith("07:")){
				Text word2 = new Text(word.toString().substring(0,3));
				context.write(word2, one);
			}else if(word.toString().startsWith("08:")){
				Text word2 = new Text(word.toString().substring(0,3));
				context.write(word2, one);
			}else if(word.toString().startsWith("09:")){
				Text word2 = new Text(word.toString().substring(0,3));
				context.write(word2, one);
			}else if(word.toString().startsWith("10:")){
				Text word2 = new Text(word.toString().substring(0,3));
				context.write(word2, one);
			}else if(word.toString().startsWith("11:")){
				Text word2 = new Text(word.toString().substring(0,3));
				context.write(word2, one);
			}else if(word.toString().startsWith("12:")){
				Text word2 = new Text(word.toString().substring(0,3));
				context.write(word2, one);
			}else if(word.toString().startsWith("13:")){
				Text word2 = new Text(word.toString().substring(0,3));
				context.write(word2, one);
			}else if(word.toString().startsWith("14:")){
				Text word2 = new Text(word.toString().substring(0,3));
				context.write(word2, one);
			}else if(word.toString().startsWith("15:")){
				Text word2 = new Text(word.toString().substring(0,3));
				context.write(word2, one);
			}else if(word.toString().startsWith("16:")){
				Text word2 = new Text(word.toString().substring(0,3));
				context.write(word2, one);
			}else if(word.toString().startsWith("17:")){
				Text word2 = new Text(word.toString().substring(0,3));
				context.write(word2, one);
			}else if(word.toString().startsWith("18:")){
				Text word2 = new Text(word.toString().substring(0,3));
				context.write(word2, one);
			}else if(word.toString().startsWith("19:")){
				Text word2 = new Text(word.toString().substring(0,3));
				context.write(word2, one);
			}else if(word.toString().startsWith("20:")){
				Text word2 = new Text(word.toString().substring(0,3));
				context.write(word2, one);
			}else if(word.toString().startsWith("21:")){
				Text word2 = new Text(word.toString().substring(0,3));
				context.write(word2, one);
			}else if(word.toString().startsWith("22:")){
				Text word2 = new Text(word.toString().substring(0,3));
				context.write(word2, one);
			}else if(word.toString().startsWith("23:")){
				Text word2 = new Text(word.toString().substring(0,3));
				context.write(word2, one);
			}
	    //}
		
      }
    }
  }

  public static class IntSumReducer
       extends Reducer<Text,IntWritable,Text,IntWritable> {
    private IntWritable result = new IntWritable();

    public void reduce(Text key, Iterable<IntWritable> values,
                       Context context
                       ) throws IOException, InterruptedException {
      int sum = 0;
      for (IntWritable val : values) {
        sum += val.get();
      }
      result.set(sum);
      context.write(key, result);
    }
  }

  public static void main(String[] args) throws Exception {
    Configuration conf = new Configuration();
    Job job = Job.getInstance(conf, "word count");
    job.setJarByClass(TimeOfDayTweets.class);
    job.setMapperClass(TokenizerMapper.class);
    job.setCombinerClass(IntSumReducer.class);
    job.setReducerClass(IntSumReducer.class);
    job.setOutputKeyClass(Text.class);
    job.setOutputValueClass(IntWritable.class);
    FileInputFormat.addInputPath(job, new Path(args[0]));
    FileOutputFormat.setOutputPath(job, new Path(args[1]));
    System.exit(job.waitForCompletion(true) ? 0 : 1);
  }
}